// Copyright (c) 2012 Cristian Patrasciuc. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include <string>

#include "base/callable.h"
#include "base/function.h"
#include "base/method.h"
#include "base/ptr/scoped_ptr.h"
#include "gtest/gtest.h"

namespace base {
namespace {

int f ()                           { return 10; }
int f1(int x)                      { return x; }
int f2(int x, int y)               { return x * y; }
int f3(int x, int y, int z)        { return x * y * z; }
int f4(int x, int y, int z, int t) { return x * y * z * t; }

class StaticHelper {
 public:
  static int f ()                           { return 10; }
  static int f1(int x)                      { return x; }
  static int f2(int x, int y)               { return x * y; }
  static int f3(int x, int y, int z)        { return x * y * z; }
  static int f4(int x, int y, int z, int t) { return x * y * z * t; }
};

class Helper {
 public:
  explicit Helper(int x) : x_(x) {}

  int f ()                           { return 10 * x_; }
  int f1(int x)                      { return x * x_; }
  int f2(int x, int y)               { return x * y * x_; }
  int f3(int x, int y, int z)        { return x * y * z * x_; }
  int f4(int x, int y, int z, int t) { return x * y * z * t * x_; }

 private:
  int x_;
};

class ConstHelper {
 public:
  explicit ConstHelper(int x) : x_(x) {}

  int f ()                           const { return 10 * x_; }
  int f1(int x)                      const { return x * x_; }
  int f2(int x, int y)               const { return x * y * x_; }
  int f3(int x, int y, int z)        const { return x * y * z * x_; }
  int f4(int x, int y, int z, int t) const { return x * y * z * t * x_; }

 private:
  int x_;
};

void GenericCallableTest(Callable<int(void)>& c,
                         Callable<int(int)>& c1,
                         Callable<int(int, int)>& c2,
                         Callable<int(int, int, int)>& c3,
                         Callable<int(int, int, int, int)>& c4) {
  EXPECT_EQ(10, c());
  EXPECT_EQ(23, c1(23));
  EXPECT_EQ(10, c2(2, 5));
  EXPECT_EQ(12, c3(2, 3, 2));
  EXPECT_EQ(24, c4(1, 2, 3, 4));
}

TEST(CallableTest, Functions) {
  Function<int(void)> func(&f);
  Function<int(int)> func1(&f1);
  Function<int(int, int)> func2(&f2);
  Function<int(int, int, int)> func3(&f3);
  Function<int(int, int, int, int)> func4(&f4);

  GenericCallableTest(func, func1, func2, func3, func4);
}

TEST(CallableTest, StaticMethods) {
  Function<int(void)> func(&StaticHelper::f);
  Function<int(int)> func1(&StaticHelper::f1);
  Function<int(int, int)> func2(&StaticHelper::f2);
  Function<int(int, int, int)> func3(&StaticHelper::f3);
  Function<int(int, int, int, int)> func4(&StaticHelper::f4);

  GenericCallableTest(func, func1, func2, func3, func4);
}

TEST(CallableTest, Methods) {
  Helper h(1);

  Method<int(Helper::*)(void)> m(&Helper::f, &h);
  Method<int(Helper::*)(int)> m1(&Helper::f1, &h);
  Method<int(Helper::*)(int, int)> m2(&Helper::f2, &h);
  Method<int(Helper::*)(int, int, int)> m3(&Helper::f3, &h);
  Method<int(Helper::*)(int, int, int, int)> m4(&Helper::f4, &h);

  GenericCallableTest(m, m1, m2, m3, m4);
}

TEST(CallableTest, ConstMethods) {
  ConstHelper h(1);

  Method<int(ConstHelper::*)(void) const> m(&ConstHelper::f, &h);
  Method<int(ConstHelper::*)(int) const> m1(&ConstHelper::f1, &h);
  Method<int(ConstHelper::*)(int, int) const> m2(&ConstHelper::f2, &h);
  Method<int(ConstHelper::*)(int, int, int) const> m3(&ConstHelper::f3, &h);
  Method<int(ConstHelper::*)(int, int, int, int) const> m4(
      &ConstHelper::f4, &h);

  GenericCallableTest(m, m1, m2, m3, m4);
}

class Base {
 public:
  virtual ~Base() {}
  virtual std::string Foo(const char* s) {
    return "This should not be called";
  }
};

class Derived : public Base {
 public:
  virtual std::string Foo(const char* s) {
    return std::string(s);
  }
};

TEST(CallableTest, VirtualMethods) {
  using base::ptr::scoped_ptr;
  scoped_ptr<Base> b(new Derived);
  Method<std::string(Base::*)(const char*)> m(&Base::Foo, Get(b));
  std::string test("test");
  EXPECT_EQ(test, m(test.c_str()));
}

}  // anonymous namespace
}  // namespace base
