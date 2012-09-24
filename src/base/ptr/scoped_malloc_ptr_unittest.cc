// Copyright (c) 2012 Cristian Patrasciuc. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include <cstdlib>

#include "base/basic_macros.h"
#include "base/ptr/scoped_malloc_ptr.h"
#include "gtest/gtest.h"

namespace base {
namespace ptr {
namespace {

TEST(ScopedMallocPtrTest, Deallocation) {
  scoped_malloc_ptr<int> ptr_x(
      static_cast<int*>(std::malloc(100 * sizeof(int))));  // NOLINT
  EXPECT_TRUE(ptr_x);
  Reset(ptr_x, static_cast<int*>(std::malloc(50 * sizeof(int))));  // NOLINT
  EXPECT_TRUE(ptr_x);
}

TEST(ScopedMallocPtrTest, DisallowCopyAndAssign) {
  scoped_malloc_ptr<int> ptr_x(
      static_cast<int*>(std::malloc(50 * sizeof(int))));  // NOLINT
  SHOULD_NOT_COMPILE(scoped_malloc_ptr<int> ptr_y(ptr_x));
  SHOULD_NOT_COMPILE(scoped_malloc_ptr<int> ptr_z = ptr_x);
}

TEST(ScopedMallocPtrTest, ArrayIndexing) {
  int value = 20;
  int index = 10;
  scoped_malloc_ptr<int> ptr(
      static_cast<int*>(std::malloc(50 * sizeof(int))));  // NOLINT
  ptr[index] = value;
  EXPECT_EQ(value, ptr[index]);
  EXPECT_EQ(value, *(Get(ptr) + index));
  scoped_malloc_ptr<const int> const_ptr(
      static_cast<const int*>(std::malloc(50 * sizeof(int))));  // NOLINT
  SHOULD_NOT_COMPILE(const_ptr[index] = value);
}

}  // anonymous namespace
}  // namespace ptr
}  // namespace base

