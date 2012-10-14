// Copyright (c) 2012 Cristian Patrasciuc. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "base/basic_macros.h"
#include "base/ptr/scoped_ptr.h"
#include "gtest/gtest.h"

namespace base {
namespace ptr {
namespace {

class Helper {
 public:
  static bool destructor_was_called;
  ~Helper() {
    destructor_was_called = true;
  }
};

bool Helper::destructor_was_called = false;

TEST(ScopedPtrTest, Deallocation) {
  Helper::destructor_was_called = false;
  {
    scoped_ptr<Helper> ptr_x(new Helper);
    EXPECT_TRUE(ptr_x);
  }
  EXPECT_TRUE(Helper::destructor_was_called);
}

TEST(ScopedPtrTest, Reset) {
  Helper::destructor_was_called = false;
  {
    scoped_ptr<Helper> ptr_x(new Helper);
    EXPECT_FALSE(Helper::destructor_was_called);
    Reset(ptr_x, new Helper);
    EXPECT_TRUE(Helper::destructor_was_called);
    Helper::destructor_was_called = false;
  }
  EXPECT_TRUE(Helper::destructor_was_called);
}

TEST(ScopedPtrTest, Release) {
  Helper::destructor_was_called = false;
  Helper* raw_ptr = NULL;
  {
    scoped_ptr<Helper> ptr(new Helper);
    raw_ptr = Release(&ptr);
    EXPECT_FALSE(ptr);
  }
  EXPECT_FALSE(Helper::destructor_was_called);
  delete raw_ptr;
  EXPECT_TRUE(Helper::destructor_was_called);
}

TEST(ScopedPtrTest, DisallowCopyAndAssign) {
  scoped_ptr<int> ptr_x(new int);
  SHOULD_NOT_COMPILE(scoped_ptr<int> ptr_y(ptr_x));
  SHOULD_NOT_COMPILE(scoped_ptr<int> ptr_z = ptr_x);
}

}  // anonymous namespace
}  // namespace ptr
}  // namespace base

