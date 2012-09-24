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

}  // anonymous namespace
}  // namespace ptr
}  // namespace base

