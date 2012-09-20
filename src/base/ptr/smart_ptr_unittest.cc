// Copyright (c) 2012 Cristian Patrasciuc. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "base/ptr/default_storage_policy.h"
#include "base/ptr/smart_ptr.h"
#include "gtest/gtest.h"
#include "array_storage_policy.h"

namespace base {
namespace ptr {
namespace {

class Helper {
 public:
  int public_member;
  int GetPrivateMember() const { return private_member_; }
  void SetPrivateMember(const int x) { private_member_ = x; }
 private:
  int private_member_;
};

TEST(SmartPtrTest, SmartPointerBehaviour) {
  SmartPtr<Helper> ptr(new Helper);
  ptr->SetPrivateMember(10);
  EXPECT_EQ(10, (*ptr).GetPrivateMember());
  ptr->public_member = 20;
  EXPECT_EQ(20, ptr->public_member);
}

TEST(SmartPtrTest, DefaultInitialization) {
  SmartPtr<Helper> ptr;
  // TODO(smart_pointer): Make conversion to NULL possible
  Helper* h = Get(ptr);
  EXPECT_EQ(NULL, h);
}

TEST(SmartPtrTest, Get) {
  int* x = new int;
  SmartPtr<int> ptr(x);
  EXPECT_EQ(x, Get(ptr));
}

TEST(SmartPtrTest, Const) {
  const int* x = NULL;
  SmartPtr<const int> ptr1(x);
  // TODO(macros): Add a macro to specify that something should not compile
  // *ptr1 = 10;  // Should not compile
  Get(ptr1);

  SmartPtr<const Helper> ptr2(new Helper);
  ptr2->GetPrivateMember();
  // ptr2->SetPrivateMember(10);  // Should not compile

  const SmartPtr<const Helper> ptr3(new Helper);
  ptr3->GetPrivateMember();
  // ptr3->SetPrivateMember(10);  // Should not compile
  // ptr3 = ptr2;  // Should not compile
  // TODO(smart_pointer): ptr2 = ptr3;
}

}  // anonymous namespace
}  // namespace ptr
}  // namespace base