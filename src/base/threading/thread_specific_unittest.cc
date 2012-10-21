// Copyright (c) 2012 Cristian Patrasciuc. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include <vector>

#include "base/bind.h"
#include "base/function.h"
#include "base/threading/atomic.h"
#include "base/threading/thread_specific.h"
#include "base/threading/thread_pool_for_unittests.h"
#include "gtest/gtest.h"

namespace base {
namespace threading {
namespace {

class IntHolder {
 public:
  static Atomic<int> delete_counter;

  ~IntHolder() {
    IntHolder::delete_counter.Increment();
  }

  int value() const {
    return value_;
  }

  void set_value(int value) {
    value_ = value;
  }

 private:
  int value_;
};

Atomic<int> IntHolder::delete_counter(0);

void ThreadSpecificAssertEquals(const ThreadSpecific<IntHolder>& t, int value) {
  EXPECT_TRUE(t.Get());
  EXPECT_EQ(value, t.Get()->value());
}

void ThreadSpecificAssertNotInitialized(const ThreadSpecific<IntHolder>& t) {
  EXPECT_EQ(NULL, t.Get());
}

void ThreadSpecificSetValue(ThreadSpecific<IntHolder>* t, int value) {
  if (!t->Get()) {
    t->Set(new IntHolder);
  }
  t->Get()->set_value(value);
}

void ThreadSpecificDelete(ThreadSpecific<IntHolder>* tsi) {
  delete tsi->Get();
  tsi->Set(NULL);
}

TEST(ThreadSpecific, Basic) {
  ThreadPoolForUnittests thread_pool;
  ThreadSpecific<IntHolder> tsi;

  thread_pool.CreateThreads();
  thread_pool.StartThreads();

  for (int i = 0; i < thread_pool.thread_count(); ++i) {
    thread_pool.SubmitTask(i, FROM_HERE,
        Bind(new Function<void(const ThreadSpecific<IntHolder>&)>(
            &ThreadSpecificAssertNotInitialized), ConstRef(&tsi)));
  }

  for (int i = 0; i < thread_pool.thread_count(); ++i) {
    thread_pool.SubmitTask(i, FROM_HERE,
        Bind(new Function<void(ThreadSpecific<IntHolder>*, int)>(
            &ThreadSpecificSetValue), &tsi, i));
  }

  for (int i = 0; i < thread_pool.thread_count(); ++i) {
    thread_pool.SubmitTask(i, FROM_HERE,
        Bind(new Function<void(const ThreadSpecific<IntHolder>&, int)>(
            &ThreadSpecificAssertEquals), ConstRef(&tsi), i));
  }

  for (int i = 0; i < thread_pool.thread_count(); ++i) {
    thread_pool.SubmitTask(i, FROM_HERE,
        Bind(new Function<void(ThreadSpecific<IntHolder>*, int)>(
            &ThreadSpecificSetValue), &tsi, 2*i));
  }

  for (int i = 0; i < thread_pool.thread_count(); ++i) {
    thread_pool.SubmitTask(i, FROM_HERE,
        Bind(new Function<void(const ThreadSpecific<IntHolder>&, int)>(
            &ThreadSpecificAssertEquals), ConstRef(&tsi), 2*i));
  }

  for (int i = 0; i < thread_pool.thread_count(); ++i) {
    thread_pool.SubmitTask(i, FROM_HERE,
        Bind(new Function<void(ThreadSpecific<IntHolder>*)>(
            &ThreadSpecificDelete), &tsi));
  }

  thread_pool.StopAndJoinThreads();
  EXPECT_EQ(thread_pool.thread_count(), IntHolder::delete_counter.Get());
}

}  // anonymous namespace
}  // namespace threading
}  // namespace base

