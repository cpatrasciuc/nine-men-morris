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

int const_ref_product(const int& x, const int& y) { return x * y; }

class Helper {
 public:
  string test_method(string s, int index) {
    return s.substr(index);
  }
};

class RefCountedHelper : public base::ptr::RefCounted<RefCountedHelper> {
 public:
  explicit RefCountedHelper(int x) : x_(x) {}
  int test_method() { return x_; }

 private:
  int x_;
};

int ref_counted_function(RefCountedHelper* h) { return h->test_method(); }

class WeakHelper : public base::ptr::SupportsWeakRef<WeakHelper> {
 public:
  static int value;
  void test_method(int x) {
    value = x;
  }
};

int WeakHelper::value;

bool weak_function(WeakHelper* h) { return h; }

class ClosureTestHelper {
 public:
  int get() const {
    return value_;
  }

  void set(int value) {
    value_ = value;
  }

 private:
  int value_;
};

TEST(BindTest, Arity1) {
  scoped_ptr<Callable<int(void)> > constant_function(
      Bind(new Function<int(int)>(&f1), 2));
  EXPECT_EQ(2, (*constant_function)());
}

TEST(BindTest, Arity2) {
  scoped_ptr<Callable<int(int)> > multiply_by_2(
      Bind(new Function<int(int, int)>(&f2), 2));
  EXPECT_EQ(6, (*multiply_by_2)(3));
  scoped_ptr<Callable<int(void)> > constant_function(
      Bind(new Function<int(int, int)>(&f2), 3, 4));
  EXPECT_EQ(12, (*constant_function)());
}

TEST(BindTest, Arity3) {
  scoped_ptr<Callable<int(int, int)> > product(
      Bind(new Function<int(int, int, int)>(&f3), 1));
  EXPECT_EQ(24, (*product)(6, 4));
  scoped_ptr<Callable<int(int)> > multiply_by_2(
      Bind(new Function<int(int, int, int)>(&f3), 1, 2));
  EXPECT_EQ(10, (*multiply_by_2)(5));
  scoped_ptr<Callable<int(void)> > constant_function(
      Bind(new Function<int(int, int, int)>(&f3), 1, 2, -1));
  EXPECT_EQ(-2, (*constant_function)());
}

TEST(BindTest, Arity4) {
  scoped_ptr<Callable<int(int, int, int)> > product3(
      Bind(new Function<int(int, int, int, int)>(&f4), 1));
  EXPECT_EQ(12, (*product3)(2, 3, 2));
  scoped_ptr<Callable<int(int, int)> > product2(
      Bind(new Function<int(int, int, int, int)>(&f4), 1, 1));
  EXPECT_EQ(12, (*product2)(6, 2));
  scoped_ptr<Callable<int(int)> > multiply_by_2(
      Bind(new Function<int(int, int, int, int)>(&f4), 1, 2, 1));
  EXPECT_EQ(10, (*multiply_by_2)(5));
  scoped_ptr<Callable<int(void)> > constant_function(
      Bind(new Function<int(int, int, int, int)>(&f4), 1, 2, 3, 4));
  EXPECT_EQ(24, (*constant_function)());
}

TEST(BindTest, Pointers) {
  int a;
  int b;
  scoped_ptr<Callable<int(void)> > a_times_b(
      Bind(new Function<int(int*, int*)>(&p2), &a, &b));
  a = 2;
  b = 3;
  EXPECT_EQ(a * b, (*a_times_b)());
  a = 5;
  b = -1;
  EXPECT_EQ(a * b, (*a_times_b)());
}

TEST(BindTest, Currying) {
  scoped_ptr<Callable<int(void)> > composed_bind(
    Bind(Bind(new Function<int(int, int)>(&f2), 2), 3));
  EXPECT_EQ(6, (*composed_bind)());
}

TEST(BindTest, Methods) {
  Helper h;
  Method<string(Helper::*)(string, int)>* method =
      new Method<string(Helper::*)(string, int)>(&Helper::test_method);
  scoped_ptr<Callable<string(int)> > c(Bind(method, &h, string("foobar")));
  EXPECT_EQ("bar", (*c)(3));
}

TEST(BindTest, Closure) {
  int x = 23;
  ClosureTestHelper cth;
  Method<void(ClosureTestHelper::*)(int)>* method =
      new Method<void(ClosureTestHelper::*)(int)>(&ClosureTestHelper::set);
  scoped_ptr<Closure> c(Bind(method, &cth, x));
  (*c)();
  EXPECT_EQ(x, cth.get());
}

TEST(BindTest, Owned) {
  int* x = new int(4);
  int y = 2;
  Function<int(int*, int*)>* function= new Function<int(int*, int*)>(&p2);
  scoped_ptr<Callable<int(int*)> > c(Bind(function, Owned(x)));
  EXPECT_EQ((*x) * y, (*c)(&y));
}

TEST(BindTest, RefCounted) {
  scoped_ptr<Callable<int(void)> > c(
      Bind(new Function<int(RefCountedHelper*)>(&ref_counted_function),
           ref_ptr<RefCountedHelper>(new RefCountedHelper(10))));
  EXPECT_EQ(10, (*c)());
  RefCountedHelper rch(20);
  Reset(c,
      Bind(new Function<int(RefCountedHelper*)>(&ref_counted_function), &rch));
  EXPECT_EQ(20, (*c)());
}

TEST(BindTest, ConstReferences) {
  int x = 2;
  int y = 3;
  scoped_ptr<Callable<int(void)> > c(
      Bind(new Function<int(const int&, const int&)>(&const_ref_product),
           ConstRef(&x), ConstRef(&y)));
  EXPECT_EQ(x * y, (*c)());
  x = 10;
  y = 10;
  EXPECT_EQ(x * y, (*c)());
}

TEST(BindTest, WeakBind) {
  scoped_ptr<Callable<bool(void)> > c;
  {
    WeakHelper wh;
    Reset(c, Bind(new Function<bool(WeakHelper*)>(&weak_function), Weak(&wh)));
    EXPECT_TRUE((*c)());
  }
  EXPECT_FALSE((*c)());
}

TEST(BindTest, WeakMethods) {
  int x = 10;
  Method<void(WeakHelper::*)(int)>* method =
      new Method<void(WeakHelper::*)(int)>(&WeakHelper::test_method);
  scoped_ptr<Callable<void(void)> > weak_method;
  WeakHelper::value = 0;
  {
    WeakHelper wh;
    Reset(weak_method, Bind(method, Weak(&wh), x));
    (*weak_method)();  // This should set the value to |x|
    EXPECT_EQ(x, WeakHelper::value);
  }
  WeakHelper::value = 0;
  (*weak_method)();  // This should be a no-op since the object is gone
  EXPECT_EQ(0, WeakHelper::value);
}

}  // anonymous namespace
}  // namespace base
