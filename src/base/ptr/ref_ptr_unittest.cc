// Copyright (c) 2012 Cristian Patrasciuc. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "base/bind.h"
#include "base/function.h"
#include "base/location.h"
#include "base/ptr/ref_counted.h"
#include "base/ptr/ref_ptr.h"
#include "base/threading/thread_pool_for_unittests.h"
#include "gtest/gtest.h"

namespace base {
namespace ptr {
namespace {

class Helper : public RefCounted<Helper> {
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

void PassByValue(ref_ptr<const Helper> value_arg) {
  const ref_ptr<const Helper> dummy(value_arg);
}

TEST(RefPtrTest, PassByValue) {
  ref_ptr<Helper> ptr(new Helper);
  PassByValue(ptr);
  EXPECT_TRUE(ptr->HasOnlyOneRef());
}

class CyclicDepsHelper : public RefCounted<CyclicDepsHelper> {
 public:
  void set_dependency(ref_ptr<CyclicDepsHelper> dep) {
    dep_ = dep;
  }

 private:
  ref_ptr<CyclicDepsHelper> dep_;
};

// This test generates a memory leak due to a cyclic dependency between
// two ref_ptr's.
TEST(RefPtrTest, DISABLED_CyclicDependency) {
  ref_ptr<CyclicDepsHelper> one = new CyclicDepsHelper;
  ref_ptr<CyclicDepsHelper> two = new CyclicDepsHelper;
  one->set_dependency(two);
  two->set_dependency(one);
}

class ThreadSafeHelper : public RefCountedThreadSafe<ThreadSafeHelper> {};

void PassByValue(ref_ptr<ThreadSafeHelper> ptr) {
  const ref_ptr<ThreadSafeHelper> dummy(ptr);
}

TEST(RefPtrTest, ThreadSafety) {
  ref_ptr<ThreadSafeHelper> ptr(new ThreadSafeHelper);
  threading::ThreadPoolForUnittests thread_pool;
  thread_pool.CreateThreads();
  for (int i = 0; i < thread_pool.thread_count(); ++i) {
    thread_pool.SubmitTask(i, FROM_HERE,
        Bind(new Function<void(ref_ptr<ThreadSafeHelper>)>(&PassByValue), ptr));
  }
  thread_pool.StartThreads();
  thread_pool.StopAndJoinThreads();
  EXPECT_TRUE(ptr->HasOnlyOneRef());
}

}  // anonymous namespace
}  // namespace ptr
}  // namespace base
