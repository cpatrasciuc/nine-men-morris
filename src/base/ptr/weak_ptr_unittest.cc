// Copyright (c) 2012 Cristian Patrasciuc. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "base/basic_macros.h"
#include "base/ptr/ref_counted.h"
#include "base/ptr/ref_ptr.h"
#include "base/ptr/supports_weak_ref.h"
#include "base/ptr/weak_ptr.h"
#include "gtest/gtest.h"

namespace base {
namespace ptr {
namespace {

class Helper : public SupportsWeakRef<Helper> {};

TEST(WeakPtrTest, NullWeakPtr) {
  weak_ptr<Helper> null_weak_ptr;
  EXPECT_FALSE(null_weak_ptr);
}

TEST(WeakPtrTest, Basic) {
  weak_ptr<Helper> wp;
  {
    Helper h;
    wp = &h;
    EXPECT_TRUE(Get(wp));
    EXPECT_EQ(&h, Get(wp));
  }
  EXPECT_FALSE(Get(wp));
}

TEST(WeakPtrTest, Multiple) {
  Helper* h = new Helper;
  weak_ptr<Helper> ptr1(h);
  weak_ptr<Helper> ptr2(h);
  EXPECT_EQ(h, Get(ptr1));
  EXPECT_EQ(h, Get(ptr2));
  delete h;
  EXPECT_FALSE(Get(ptr1));
  EXPECT_FALSE(Get(ptr2));
}

TEST(WeakPtrTest, Assignment) {
  Helper h;
  weak_ptr<Helper> ptr1(&h);
  weak_ptr<Helper> ptr2 = ptr1;
  EXPECT_EQ(ptr1, ptr2);
}

class RefCountedHelper : public RefCounted<RefCountedHelper>,
                         public SupportsWeakRef<RefCountedHelper> {
 public:
  RefCountedHelper() {}
  ~RefCountedHelper() {}

  void set_dependency(const weak_ptr<RefCountedHelper>& dep) {
    dep_ = dep;
  }

 private:
  weak_ptr<RefCountedHelper> dep_;

  DISALLOW_COPY_AND_ASSIGN(RefCountedHelper);
};

TEST(WeakPtrTest, Lock) {
  ref_ptr<RefCountedHelper> strong(new RefCountedHelper);
  weak_ptr<RefCountedHelper> weak(Get(strong));
  EXPECT_TRUE(Get(weak));
  EXPECT_TRUE(weak);
  {
    ref_ptr<RefCountedHelper> locked_ptr = Lock(weak);
    EXPECT_TRUE(locked_ptr);
    EXPECT_TRUE(Get(weak));
    strong = NULL;
    EXPECT_TRUE(locked_ptr);
    EXPECT_TRUE(Get(weak));
  }
  EXPECT_FALSE(Get(weak));
  ref_ptr<RefCountedHelper> invalid = Lock(weak);
  EXPECT_FALSE(invalid);
}

// This should not cause any memory leaks
TEST(WeakPtrTest, CyclicDependency) {
  ref_ptr<RefCountedHelper> one(new RefCountedHelper);
  ref_ptr<RefCountedHelper> two(new RefCountedHelper);
  one->set_dependency(Get(two));
  two->set_dependency(Get(one));
}

class Base : public SupportsWeakRef<Base> {};
class Derived : public Base {};

TEST(WeakPtrTest, Polymorphism) {
  Derived d;
  weak_ptr<Base> base_ptr(&d);
  EXPECT_EQ(&d, Get(base_ptr));
}

}  // anonymous namespace
}  // namespace ptr
}  // namespace base
