// Copyright (c) 2012 Cristian Patrasciuc. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "base/basic_macros.h"
#include "base/ptr/default_storage_policy.h"
#include "base/ptr/smart_ptr.h"
#include "gtest/gtest.h"

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
  SHOULD_NOT_COMPILE(*ptr1 = 10);
  Get(ptr1);

  SmartPtr<const Helper> ptr2(new Helper);
  ptr2->GetPrivateMember();
  SHOULD_NOT_COMPILE(ptr2->SetPrivateMember(10));

  const SmartPtr<const Helper> ptr3(new Helper);
  ptr3->GetPrivateMember();
  SHOULD_NOT_COMPILE(ptr3->SetPrivateMember(10));
  SHOULD_NOT_COMPILE(ptr3 = ptr2);
  // TODO(smart_pointer): ptr2 = ptr3;
}

TEST(SmartPtrTest, NullComparison) {
  SmartPtr<int> null_ptr;
  EXPECT_TRUE(!null_ptr);
  EXPECT_FALSE(null_ptr);

  SmartPtr<int> non_null_ptr(new int);
  EXPECT_FALSE(!non_null_ptr);
  // EXPECT_TRUE(non_null_ptr);
}

class IntPtr {
 public:
  explicit IntPtr(int* p) : p_(p) {}
  operator int*() const {
    return p_;
  }
 private:
  int* p_;
};

TEST(SmartPtrTest, EqualityAndInequality) {
  int a = 10;
  int b = 20;
  int* ptr_a = &a;
  int* ptr_b = &b;
  SmartPtr<int> smart_ptr_a(&a);
  SmartPtr<int> smart_ptr_b(&b);
  SmartPtr<int> smart_ptr_a2(&a);
  IntPtr helper_a(&a);
  IntPtr helper_b(&b);
  EXPECT_TRUE(smart_ptr_a == ptr_a);
  EXPECT_TRUE(smart_ptr_a == smart_ptr_a2);
  EXPECT_TRUE(smart_ptr_a == helper_a);
  EXPECT_TRUE(ptr_a == smart_ptr_a);
  EXPECT_TRUE(helper_a == smart_ptr_a);
  EXPECT_TRUE(smart_ptr_a != ptr_b);
  EXPECT_TRUE(smart_ptr_a != smart_ptr_b);
  EXPECT_TRUE(smart_ptr_a != helper_b);
  EXPECT_TRUE(ptr_b != smart_ptr_a);
  EXPECT_TRUE(helper_b != smart_ptr_a);
  EXPECT_TRUE(smart_ptr_a != NULL);
  EXPECT_TRUE(SmartPtr<int>() == NULL);
  EXPECT_TRUE(NULL != smart_ptr_a);
  EXPECT_TRUE(NULL == SmartPtr<int>());
}

}  // anonymous namespace
}  // namespace ptr
}  // namespace base
