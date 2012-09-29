// Copyright (c) 2012 Cristian Patrasciuc. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "base/ptr/ref_counted.h"
#include "base/ptr/ref_ptr.h"
#include "base/ptr/supports_weak_ref.h"
#include "base/ptr/weak_ptr.h"
#include "gtest/gtest.h"

namespace base {
namespace ptr {
namespace {

class Helper : public RefCounted, public SupportsWeakRef<Helper> {};

TEST(WeakPtrTest, NullWeakPtr) {
  weak_ptr<Helper> null_weak_ptr;
  EXPECT_FALSE(null_weak_ptr.IsValid());
  EXPECT_FALSE(null_weak_ptr);
}

TEST(WeakPtrTest, Multiple) {
  ref_ptr<Helper> strong(new Helper);
  weak_ptr<Helper> ptr1 = strong;
  weak_ptr<Helper> ptr2 = strong;
  EXPECT_EQ(Get(strong), Get(ptr1));
  EXPECT_EQ(Get(strong), Get(ptr2));
  strong = NULL;
  EXPECT_FALSE(Get(ptr1));
  EXPECT_FALSE(Get(ptr2));
}

TEST(WeakPtrTest, Assignment) {
  ref_ptr<Helper> strong(new Helper);
  weak_ptr<Helper> ptr1(strong);
  weak_ptr<Helper> ptr2 = ptr1;
  EXPECT_EQ(ptr1, ptr2);
}

TEST(WeakPtrTest, Lock) {
  ref_ptr<Helper> strong(new Helper);
  weak_ptr<Helper> weak(strong);
  EXPECT_TRUE(weak.IsValid());
  EXPECT_TRUE(weak);
  {
    ref_ptr<Helper> locked_ptr = weak.Lock();
    EXPECT_TRUE(locked_ptr);
    EXPECT_TRUE(weak.IsValid());
    strong = NULL;
    EXPECT_TRUE(locked_ptr);
    EXPECT_TRUE(weak.IsValid());
  }
  EXPECT_FALSE(weak.IsValid());
  ref_ptr<Helper> invalid = weak.Lock();
  EXPECT_FALSE(invalid);
}

class CyclicDepsHelper : public RefCounted,
                         public SupportsWeakRef<CyclicDepsHelper> {
 public:
  void set_dependency(const weak_ptr<CyclicDepsHelper>& dep) {
    dep_ = dep;
  }

 private:
  weak_ptr<CyclicDepsHelper> dep_;
};

// This should not cause any memory leaks
TEST(WeakPtrTest, CyclicDependency) {
  ref_ptr<CyclicDepsHelper> one(new CyclicDepsHelper);
  ref_ptr<CyclicDepsHelper> two(new CyclicDepsHelper);
  one->set_dependency(two);
  two->set_dependency(one);
}

class Base : public SupportsWeakRef<Base>, public RefCounted {};
class Derived : public Base {};

TEST(WeakPtrTest, Polymorphism) {
  ref_ptr<Derived> derived_ptr(new Derived);
  weak_ptr<Base> base_ptr(derived_ptr);
  EXPECT_EQ(Get(derived_ptr), Get(base_ptr.Lock()));
}

}  // anonymous namespace
}  // namespace ptr
}  // namespace base
