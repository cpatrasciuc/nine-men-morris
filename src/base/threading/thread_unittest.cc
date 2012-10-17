// Copyright (c) 2012 Cristian Patrasciuc. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include <vector>

#include "base/bind.h"
#include "base/location.h"
#include "base/log.h"
#include "base/method.h"
#include "base/string_util.h"
#include "base/threading/atomic.h"
#include "base/threading/lock.h"
#include "base/threading/thread.h"
#include "gtest/gtest.h"

namespace base {
namespace threading {
namespace {

const int kThreadCount = 10;

template <class LockImpl>
class LockCounter {
 public:
  LockCounter() : counter_(0), counter_lock_(new LockImpl) {}

  void IncrementCounter() {
    ScopedGuard _(&counter_lock_);
    ++counter_;
  }

  int value() const { return counter_; }

 private:
  int counter_;
  Lock counter_lock_;
};

class AtomicCounter {
 public:
  AtomicCounter() : counter_(0) {}
  void IncrementCounter() { counter_.Increment(); }
  int value() const { return counter_.Get(); }

 private:
  Atomic<int> counter_;
};

template <typename T>
class ThreadTest : public ::testing::Test {
 public:
  virtual void SetUp() {
    for (int i = 0; i < kThreadCount; ++i) {
      threads_.push_back(new Thread("Test Thread " + ToString(i+1)));
    }
    for (size_t i = 0; i < threads_.size(); ++i) {
      EXPECT_FALSE(threads_[i]->is_running());
      threads_[i]->Start();
      EXPECT_TRUE(threads_[i]->is_running());
      EXPECT_FALSE(Thread::CurrentlyOn(threads_[i]));
    }
  }

  virtual void TearDown() {
    for (size_t i = 0; i < threads_.size(); ++i) {
      threads_[i]->SubmitQuitTaskAndJoin();
      delete threads_[i];
    }
    EXPECT_EQ(thread_count(), counter_.value());
  }

  void SubmitTask(int thread_number, Location loc, Closure* closure) {
    DCHECK_LT(thread_number, thread_count());
    threads_[thread_number]->SubmitTask(loc, closure);
  }

 protected:
  int thread_count() const {
    return threads_.size();
  }

  T* counter() {
    return &counter_;
  }

 private:
  std::vector<Thread*> threads_;
  T counter_;
};

typedef ::testing::Types<LockCounter<MutexLockImpl>,
                         LockCounter<SpinLockImpl>,
                         AtomicCounter> CounterTypes;
TYPED_TEST_CASE(ThreadTest, CounterTypes);

TYPED_TEST(ThreadTest, Counting) {
  for (int i = 0; i < this->thread_count(); ++i) {
    this->SubmitTask(i, FROM_HERE,
        Bind(new Method<void(TypeParam::*)(void)>(&TypeParam::IncrementCounter),
            this->counter()));
  }
}

#if defined(DEBUG_MODE)

TEST(ThreadDeathTest, Join) {
  ::testing::FLAGS_gtest_death_test_style = "threadsafe";
  Thread thread("Test thread");
  thread.Start();
  thread.QuitWhenIdle();
  thread.Join();
  ASSERT_DEATH(thread.Join(), "Error while joining thread");
}

TEST(ThreadDeathTest, SelfJoin) {
  ::testing::FLAGS_gtest_death_test_style = "threadsafe";
  Thread thread("Test thread");
  thread.Start();
  Closure* self_join =
      Bind(new Method<void(Thread::*)(void)>(&Thread::Join), &thread);
  ASSERT_DEATH({
        thread.SubmitTask(FROM_HERE, self_join);
        thread.Join();
      },
      "Error while joining thread");
  delete self_join;
  thread.SubmitQuitTaskAndJoin();
}

TEST(ThreadDeathTest, MultipleJoins) {
  ::testing::FLAGS_gtest_death_test_style = "threadsafe";
  Thread thread("Test thread");
  Thread join_thread("Thread that calls Join()");
  thread.Start();
  join_thread.Start();
  Closure* join =
      Bind(new Method<void(Thread::*)(void)>(&Thread::Join), &thread);
  ASSERT_DEATH({
        join_thread.SubmitTask(FROM_HERE, join);
        thread.Join();
      },
      "Error while joining thread");
  delete join;
  join_thread.SubmitQuitTaskAndJoin();
  thread.SubmitQuitTaskAndJoin();
}

TEST(ThreadDeathTest, NotJoined) {
  ::testing::FLAGS_gtest_death_test_style = "threadsafe";
  {
    Thread not_started("Not started thread");
    EXPECT_FALSE(not_started.is_running());
  }
  ASSERT_DEATH({
      Thread thread("Test thread");
      thread.Start();
    },
    "destroyed before being joined");
}

TEST(ThreadDeathTest, JoinBeforeStart) {
  ::testing::FLAGS_gtest_death_test_style = "threadsafe";
  Thread thread("Test thread");
  ASSERT_DEATH(thread.Join(), "not started");
}

TEST(ThreadDeathTest, MultipleStarts) {
  ::testing::FLAGS_gtest_death_test_style = "threadsafe";
  Thread thread("Test thread");
  thread.Start();
  ASSERT_DEATH(thread.Start(), "already running");
  thread.SubmitQuitTaskAndJoin();
  ASSERT_DEATH({
      thread.Start();
      thread.SubmitQuitTaskAndJoin();
    },
    "already run and joined");
}

#endif  // defined(DEBUG_MODE)

}  // anonymous namespace
}  // namespace threading
}  // namespace base
