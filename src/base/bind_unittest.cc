// Copyright (c) 2012 Cristian Patrasciuc. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include <string>

#include "base/bind.h"
#include "base/function.h"
#include "base/method.h"
#include "base/ptr/ref_counted.h"
#include "base/ptr/ref_ptr.h"
#include "base/ptr/scoped_ptr.h"
#include "base/ptr/supports_weak_ref.h"
#include "base/ptr/weak_ptr.h"
#include "gtest/gtest.h"
#include "ptr/scoped_array_ptr.h"

namespace base {
namespace {

using base::ptr::ref_ptr;
using base::ptr::scoped_ptr;
using base::ptr::weak_ptr;
using std::string;

int f1(int x) { return x; }
int f2(int x, int y) { return x * y; }
int f3(int x, int y, int z) { return x * y * z; }
int f4(int x, int y, int z, int t) { return x * y * z * t; }

int p2(int* x, int* y) { return (*x) * (*y); }

class Helper {
 public:
  string test_method(string s, int index) {
    return s.substr(index);
  }
};

class RefCountedHelper : public base::ptr::RefCounted {
 public:
  explicit RefCountedHelper(int x) : x_(x) {}
  int test_method() { return x_; }

 private:
  int x_;
};

int ref_counted_function(RefCountedHelper* h) { return h->test_method(); }

class WeakHelper : public base::ptr::SupportsWeakRef<WeakHelper> {};

bool weak_function(WeakHelper* h) { return h; }

class ClosureTestHelper {
 public:
  int get_value() const {
    return value_;
  }

  void set_value(int value) {
    value_ = value;
  }

 private:
  int value_;
};

TEST(BindTest, Arity1) {
  Function<int(int)> func(&f1);
  Callable<int(void)>* constant_function = Bind(&func, 2);
  EXPECT_EQ(2, (*constant_function)());
  delete constant_function;
}

TEST(BindTest, Arity2) {
  Function<int(int, int)> product(&f2);
  Callable<int(int)>* multiply_by_2 = Bind(&product, 2);
  EXPECT_EQ(6, (*multiply_by_2)(3));
  delete multiply_by_2;

  Callable<int(void)>* constant_function = Bind(&product, 3, 4);
  EXPECT_EQ(12, (*constant_function)());
  delete constant_function;
}

TEST(BindTest, Arity3) {
  Function<int(int, int, int)> func3(&f3);

  Callable<int(int, int)>* product = Bind(&func3, 1);
  EXPECT_EQ(24, (*product)(6, 4));
  delete product;

  Callable<int(int)>* multiply_by_2 = Bind(&func3, 1, 2);
  EXPECT_EQ(10, (*multiply_by_2)(5));
  delete multiply_by_2;

  Callable<int(void)>* constant_function = Bind(&func3, 1, 2, -1);
  EXPECT_EQ(-2, (*constant_function)());
  delete constant_function;
}

TEST(BindTest, Arity4) {
  Function<int(int, int, int, int)> product4(&f4);

  Callable<int(int, int, int)>* product3 = Bind(&product4, 1);
  EXPECT_EQ(12, (*product3)(2, 3, 2));
  delete product3;

  Callable<int(int, int)>* product2 = Bind(&product4, 1, 1);
  EXPECT_EQ(12, (*product2)(6, 2));
  delete product2;

  Callable<int(int)>* multiply_by_2 = Bind(&product4, 1, 2, 1);
  EXPECT_EQ(10, (*multiply_by_2)(5));
  delete multiply_by_2;

  Callable<int(void)>* constant_function = Bind(&product4, 1, 2, 3, 4);
  EXPECT_EQ(24, (*constant_function)());
  delete constant_function;
}

TEST(BindTest, Pointers) {
  int a;
  int b;
  Function<int(int*, int*)> product2(&p2);
  Callable<int(void)>* a_times_b = Bind(&product2, &a, &b);
  a = 2;
  b = 3;
  EXPECT_EQ(a * b, (*a_times_b)());
  a = 5;
  b = -1;
  EXPECT_EQ(a * b, (*a_times_b)());
  delete a_times_b;
}

TEST(BindTest, Currying) {
  Function<int(int, int)> func2(&f2);
  // TODO(bind): Implement an automatic deletion mechanism for binders
  Callable<int(int)>* simple_bind = Bind(&func2, 2);
  Callable<int(void)>* composed_bind = Bind(simple_bind, 3);
  EXPECT_EQ(6, (*composed_bind)());
  delete simple_bind;
  delete composed_bind;
}

TEST(BindTest, Methods) {
  Helper h;
  Method<string(Helper::*)(string, int)> method(&Helper::test_method);
  Callable<string(int)>* c = Bind(&method, &h, string("foobar"));
  EXPECT_EQ("bar", (*c)(3));
  delete c;
}

TEST(BindTest, Closure) {
  using base::ptr::scoped_ptr;
  int x = 23;
  ClosureTestHelper cth;
  Method<void(ClosureTestHelper::*)(int)> m(&ClosureTestHelper::set_value);
  scoped_ptr<Closure> c(Bind(&m, &cth, x));
  (*c)();
  EXPECT_EQ(x, cth.get_value());
}

TEST(BindTest, Owned) {
  int* x = new int(4);
  int y = 2;
  Function<int(int*, int*)> func(&p2);
  scoped_ptr<Callable<int(int*)> > c(Bind(&func, Owned(x)));
  EXPECT_EQ((*x) * y, (*c)(&y));
}

TEST(BindTest, RefCounted) {
  Function<int(RefCountedHelper*)> func(&ref_counted_function);
  scoped_ptr<Callable<int(void)> > c(
      Bind(&func, ref_ptr<RefCountedHelper>(new RefCountedHelper(10))));
  EXPECT_EQ(10, (*c)());
  RefCountedHelper rch(20);
  Reset(c, Bind(&func, &rch));
  EXPECT_EQ(20, (*c)());
}

TEST(BindTest, WeakBind) {
  Function<bool(WeakHelper*)> func(&weak_function);
  scoped_ptr<Callable<bool(void)> > c;
  {
    WeakHelper wh;
    Reset(c, Bind(&func, Weak(&wh)));
    EXPECT_TRUE((*c)());
  }
  EXPECT_FALSE((*c)());
}

}  // anonymous namespace
}  // namespace base
