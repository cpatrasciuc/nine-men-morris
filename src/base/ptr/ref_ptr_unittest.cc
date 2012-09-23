// Copyright (c) 2012 Cristian Patrasciuc. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "base/ptr/ref_counted.h"
#include "base/ptr/ref_ptr.h"
#include "gtest/gtest.h"

namespace base {
namespace ptr {
namespace {

class Helper : public RefCounted {
 public:
  bool ConstMethod() const {
    return true;
  }
  void NonConstMethod() {}
};

TEST(RefPtrTest, Initialization) {
  ref_ptr<Helper> ptr(new Helper);
  ref_ptr<Helper> null_ptr;
  ref_ptr<Helper> ptr2(ref_ptr<Helper>(new Helper));
}

TEST(RefPtrTest, Constness) {
  ref_ptr<const Helper> ptr_to_const(new Helper);
  EXPECT_TRUE(ptr_to_const->ConstMethod());
  SHOULD_NOT_COMPILE(ptr_to_const->NonConstMethod());
}

TEST(RefPtrTest, Assignment) {
  ref_ptr<Helper> ptr1;
  ptr1 = new Helper;
  ref_ptr<Helper> ptr2(new Helper);
  ptr1 = ptr2;
  ptr1 = NULL;
  ref_ptr<Helper> ptr3;
  ptr3 = NULL;
}

TEST(RefPtrTest, SelfAssignment) {
  ref_ptr<Helper> ptr1(new Helper);
  ptr1 = ptr1;
}

TEST(RefPtrTest, ReleaseManagement) {
  Helper* h1 = new Helper;
  Helper* h2 = NULL;
  {
    ref_ptr<Helper> ptr = h1;
    EXPECT_EQ(h1, Get(ptr));
    h2 = Release(ptr);
    EXPECT_EQ(h1, h2);
  }
  // This should crash if ref_ptr still owned the object and deallocated it
  EXPECT_EQ(h1, h2);
  EXPECT_TRUE(h2->HasOnlyOneRef());
  h2->Release();
  delete h2;
}

}  // anonymous namespace
}  // namespace ptr
}  // namespace base
