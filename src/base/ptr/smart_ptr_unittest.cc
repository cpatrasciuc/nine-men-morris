// Copyright (c) 2012 Cristian Patrasciuc. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include <algorithm>
#include <vector>

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
  Helper helper;
  SmartPtr<Helper> ptr(&helper);
  ptr->SetPrivateMember(10);
  EXPECT_EQ(10, (*ptr).GetPrivateMember());
  ptr->public_member = 20;
  EXPECT_EQ(20, ptr->public_member);
}

TEST(SmartPtrTest, DefaultInitialization) {
  SmartPtr<Helper> ptr;
  EXPECT_EQ(static_cast<Helper*>(NULL), ptr);
  Helper* h = Get(ptr);
  EXPECT_EQ(NULL, h);
}

TEST(SmartPtrTest, Get) {
  int x;
  int* ptr_x = &x;
  SmartPtr<int> smart_ptr(ptr_x);
  EXPECT_EQ(ptr_x, Get(smart_ptr));
}

TEST(SmartPtrTest, Const) {
  const int* x = NULL;
  SmartPtr<const int> ptr1(x);
  SHOULD_NOT_COMPILE(*ptr1 = 10);
  Get(ptr1);

  Helper helper;
  SmartPtr<const Helper> ptr2(&helper);
  ptr2->GetPrivateMember();
  SHOULD_NOT_COMPILE(ptr2->SetPrivateMember(10));

  const SmartPtr<const Helper> ptr3(&helper);
  ptr3->GetPrivateMember();
  SHOULD_NOT_COMPILE(ptr3->SetPrivateMember(10));
  SHOULD_NOT_COMPILE(ptr3 = ptr2);
  ptr2 = ptr3;
}

TEST(SmartPtrTest, NullComparison) {
  SmartPtr<int> null_ptr;
  EXPECT_TRUE(!null_ptr);
  EXPECT_FALSE(null_ptr);

  int x;
  SmartPtr<int> non_null_ptr(&x);
  EXPECT_FALSE(!non_null_ptr);
  EXPECT_TRUE(non_null_ptr);
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
  const int* ptr_b = &b;
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

// These two operators have to be defined outside of the anonymous namespace,
// otherwise they won't be visible at link-time.

bool operator<(const SmartPtr<int>& lhs, const int* rhs) {
  return Get(lhs) < rhs;
}

bool operator<(const int* lhs, const SmartPtr<int>& rhs) {
  return lhs < Get(rhs);
}

namespace {

template<class SmartPtrType, class RawPointerType>
void OrderingOperatorsTest() {
  int v[] = {0, 1, 2, 3};
  SmartPtrType first(&v[0]);
  const SmartPtrType second(&v[1]);
  RawPointerType p(v);
  EXPECT_TRUE(first < second);
  EXPECT_TRUE(first < (p + 1));
  EXPECT_TRUE(p < second);
  EXPECT_FALSE(first > second);
  EXPECT_FALSE(first > p);
  EXPECT_TRUE((p + 2) > first);
  EXPECT_FALSE(first >= second);
  EXPECT_TRUE(first >= p);
  EXPECT_TRUE((p + 2) >= first);
  EXPECT_TRUE(first <= second);
  EXPECT_TRUE(first <= p);
  EXPECT_FALSE((p + 2) <= first);
}

TEST(SmartPtrTest, Ordering) {
  OrderingOperatorsTest<SmartPtr<int>, int*>();
  OrderingOperatorsTest<const SmartPtr<int>, int*>();
  OrderingOperatorsTest<SmartPtr<int>, const int*>();
  OrderingOperatorsTest<const SmartPtr<int>, const int*>();
  OrderingOperatorsTest<SmartPtr<int>, int*>();
}

TEST(SmartPtrTest, Copy) {
  int x = 69;
  SmartPtr<int> smart_ptr_1(&x);
  SmartPtr<int> smart_ptr_2(smart_ptr_1);
  EXPECT_EQ(smart_ptr_1, smart_ptr_2);
  EXPECT_EQ(x, *smart_ptr_2);
}

TEST(SmartPtrTest, Assignment) {
  int x = 10;
  SmartPtr<int> ptr1(&x);
  SmartPtr<int> ptr2;
  ptr2 = ptr1;
  EXPECT_EQ(&x, Get(ptr2));
  SmartPtr<const int> ptr4;
  ptr4 = ptr2;
  EXPECT_EQ(&x, Get(ptr4));
  SHOULD_NOT_COMPILE(ptr2 = ptr4);
  SHOULD_NOT_COMPILE(ptr1 = &x);
}

TEST(SmartPtrTest, StdLess) {
  int a[] = {120, 34, 35, 26};
  std::vector<SmartPtr<int> > v;
  for (int i = arraysize(a) - 1; i >= 0; --i) {
    v.push_back(SmartPtr<int>(&a[i]));
  }
  std::sort(v.begin(), v.end());
  for (unsigned int i = 0; i < v.size(); ++i) {
    EXPECT_EQ(a[i], *(v[i]));
  }
}

}  // anonymous namespace
}  // namespace ptr
}  // namespace base
