// Copyright (c) 2012 Cristian Patrasciuc. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "base/basic_macros.h"
#include "base/ptr/scoped_array_ptr.h"
#include "gtest/gtest.h"

namespace base {
namespace ptr {
namespace {

TEST(ScopedArrayPtrTest, Deallocation) {
  scoped_array_ptr<int> ptr_x(new int[100]);
  EXPECT_TRUE(ptr_x);
  Reset(ptr_x, new int[50]);
  EXPECT_TRUE(ptr_x);
}

TEST(ScopedArrayPtrTest, DisallowCopyAndAssign) {
  scoped_array_ptr<int> ptr_x(new int[50]);
  SHOULD_NOT_COMPILE(scoped_array_ptr<int> ptr_y(ptr_x));
  SHOULD_NOT_COMPILE(scoped_array_ptr<int> ptr_z = ptr_x);
}

TEST(ScopedArrayPtrTest, ArrayIndexing) {
  int value = 20;
  int index = 10;
  scoped_array_ptr<int> ptr(new int[50]);
  ptr[index] = value;
  EXPECT_EQ(value, ptr[index]);
  EXPECT_EQ(value, *(Get(ptr) + index));
  scoped_array_ptr<const int> const_ptr(new int[50]);
  SHOULD_NOT_COMPILE(const_ptr[index] = value);
}

}  // anonymous namespace
}  // namespace ptr
}  // namespace base


