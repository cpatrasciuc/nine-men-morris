// Copyright (c) 2012 Cristian Patrasciuc. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include <ctime>

#include "base/basic_macros.h"
#include "base/bind.h"
#include "base/location.h"
#include "base/log.h"
#include "base/method.h"
#include "base/ptr/scoped_ptr.h"
#include "base/threading/condition_variable.h"
#include "base/threading/lock.h"
#include "base/threading/thread_pool_for_unittests.h"
#include "gtest/gtest.h"

namespace base {
namespace threading {
namespace {

using base::ptr::scoped_ptr;

class ConditionVariableTest : public ::testing::Test {
 public:
  virtual void SetUp() {
    Reset(lock_, new Lock(new MutexLockImpl));
    Reset(cv_, new ConditionVariable(Get(lock_)));
    counter_ = 0;
    thread_pool_.CreateThreads();
    thread_pool_.StartThreads();
  }

  virtual void TearDown() {
    thread_pool_.StopAndJoinThreads();
  }

  void WaitForCounterValueAndBroadcast(int expected_value) {
    ScopedGuard _(lock());
    while (counter_ != expected_value) {
      cv_->Wait();
    }
    --counter_;
    cv_->Broadcast();
  }

  void WaitForCounterValueAndSignal(int expected_value) {
    ScopedGuard _(lock());
    while (counter_ != expected_value) {
      cv_->Wait();
      if (counter_ != expected_value) {
        cv_->Signal();
      }
    }
    --counter_;
    cv_->Signal();
  }

  int counter() const {
    return counter_;
  }

  void set_counter(const int x) {
    counter_ = x;
  }

  ThreadPoolForUnittests& thread_pool() {
    return thread_pool_;
  }

  Lock* lock() {
    return Get(lock_);
  }

  ConditionVariable* condition_variable() {
    return Get(cv_);
  }

 private:
  ThreadPoolForUnittests thread_pool_;
  scoped_ptr<Lock> lock_;
  scoped_ptr<ConditionVariable> cv_;
  int counter_;
};

TEST_F(ConditionVariableTest, Broadcast) {
  set_counter(thread_pool().thread_count() - 1);
  for (int i = 0; i < thread_pool().thread_count(); ++i) {
    thread_pool().SubmitTask(i, FROM_HERE,
        Bind(new Method<void(ConditionVariableTest::*)(int)>(
            &ConditionVariableTest::WaitForCounterValueAndBroadcast), this, i));
  }
}

TEST_F(ConditionVariableTest, Signal) {
  for (int i = 0; i < thread_pool().thread_count(); ++i) {
    thread_pool().SubmitTask(i, FROM_HERE,
        Bind(new Method<void(ConditionVariableTest::*)(int)>(
            &ConditionVariableTest::WaitForCounterValueAndSignal), this, i));
  }
  {
    ScopedGuard _(lock());
    set_counter(thread_pool().thread_count() - 1);
    condition_variable()->Signal();
  }
}

TEST_F(ConditionVariableTest, TimedWait) {
  Lock lock;
  ConditionVariable cv(&lock);
  timespec ts;
  ts.tv_sec = 1;
  ts.tv_nsec = 0;
  lock.Acquire();
  EXPECT_FALSE(cv.TimedWait(ts));
  lock.Release();
}

TEST(ConditionVariableDeathTest, DEBUG_ONLY_TEST(NoLockAcquired)) {
  ::testing::FLAGS_gtest_death_test_style = "threadsafe";
  Lock lock;
  ConditionVariable cv(&lock);
  timespec ts;
  ts.tv_sec = 1;
  ts.tv_nsec = 0;
  ASSERT_DEATH(cv.Wait(), "");
  ASSERT_DEATH(cv.TimedWait(ts), "");
  ASSERT_DEATH(cv.Signal(), "");
  ASSERT_DEATH(cv.Broadcast(), "");
}

}  // anonymous namespace
}  // namespace threading
}  // namespace base
