// Copyright (c) 2012 Cristian Patrasciuc. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "base/callable.h"
#include "base/function.h"
#include "gtest/gtest.h"

namespace base {

int f ()                           { return 69; }
int f1(int x)                      { return x; }
int f2(int x, int y)               { return x * y; }
int f3(int x, int y, int z)        { return x * y * z; }
int f4(int x, int y, int z, int t) { return x * y * z * t; }

TEST(FunctionTest, Basic) {
  Function<int(void)> func(&f);
  Function<int(int)> func1(&f1);
  Function<int(int, int)> func2(&f2);
  Function<int(int, int, int)> func3(&f3);
  Function<int(int, int, int, int)> func4(&f4);

  Callable<int(void)>& c = func;
  Callable<int(int)>& c1 = func1;
  Callable<int(int, int)>& c2 = func2;
  Callable<int(int, int, int)>& c3 = func3;
  Callable<int(int, int, int, int)>& c4 = func4;

  EXPECT_EQ(69, c());
  EXPECT_EQ(23, c1(23));
  EXPECT_EQ(10, c2(2, 5));
  EXPECT_EQ(12, c3(2, 3, 2));
  EXPECT_EQ(24, c4(1, 2, 3, 4));
}

class StaticHelper {
 public:
  static int f ()                           { return 69; }
  static int f1(int x)                      { return x; }
  static int f2(int x, int y)               { return x * y; }
  static int f3(int x, int y, int z)        { return x * y * z; }
  static int f4(int x, int y, int z, int t) { return x * y * z * t; }
};

TEST(FunctionTest, StaticMethods) {
  Function<int(void)> func(&StaticHelper::f);
  Function<int(int)> func1(&StaticHelper::f1);
  Function<int(int, int)> func2(&StaticHelper::f2);
  Function<int(int, int, int)> func3(&StaticHelper::f3);
  Function<int(int, int, int, int)> func4(&StaticHelper::f4);

  Callable<int(void)>& c = func;
  Callable<int(int)>& c1 = func1;
  Callable<int(int, int)>& c2 = func2;
  Callable<int(int, int, int)>& c3 = func3;
  Callable<int(int, int, int, int)>& c4 = func4;

  EXPECT_EQ(69, c());
  EXPECT_EQ(23, c1(23));
  EXPECT_EQ(10, c2(2, 5));
  EXPECT_EQ(12, c3(2, 3, 2));
  EXPECT_EQ(24, c4(1, 2, 3, 4));
}

}  // namespace base
