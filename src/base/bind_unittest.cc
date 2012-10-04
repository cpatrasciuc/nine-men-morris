// Copyright (c) 2012 Cristian Patrasciuc. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include <string>

#include "base/bind.h"
#include "base/function.h"
#include "base/method.h"
#include "gtest/gtest.h"

namespace base {
namespace {

using std::string;

int f1(int x) { return x; }
int f2(int x, int y) { return x * y; }
int f3(int* x, int* y, int z) { return (*x) * (*y) * z; }
int f4(int x, int y, int z, int t) { return x * y * z * t; }

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
  Function<int(int*, int*, int)> func3(&f3);
  int a = 1;
  int b = 2;
  Callable<int(int)>* multiply_by_b = Bind(&func3, &a, &b);
  EXPECT_EQ(10, (*multiply_by_b)(5));
  b = 3;
  EXPECT_EQ(15, (*multiply_by_b)(5));
  delete multiply_by_b;

  a = 2;
  b = 3;
  Callable<int(int*, int)>* double_product = Bind(&func3, &a);
  EXPECT_EQ(24, (*double_product)(&b, 4));
  delete double_product;

  Callable<int(void)>* a_times_b = Bind(&func3, &a, &b, 1);
  a = 5;
  b = 2;
  EXPECT_EQ(a * b, (*a_times_b)());
  delete a_times_b;
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

TEST(BindTest, Currying) {
  Function<int(int, int)> func2(&f2);
  // TODO(bind): Implement an automatic deletion mechanism for binders
  Callable<int(int)>* tmp = Bind(&func2, 2);
  Callable<int(void)>* final = Bind(tmp, 3);
  EXPECT_EQ(6, (*final)());
  delete tmp;
  delete final;
}

class Helper {
 public:
  string m2(string s, int index) {
    return s.substr(index);
  }
};

TEST(BindTest, Methods) {
  Helper h;
  Method<string(Helper::*)(string, int)> m(&Helper::m2, &h);
  Callable<string(int)>* c = Bind(&m, string("foobar"));
  EXPECT_EQ("bar", (*c)(3));
  delete c;
}

}  // anonymous namespace
}  // namespace base
