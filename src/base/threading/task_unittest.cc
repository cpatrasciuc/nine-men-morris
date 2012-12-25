// Copyright (c) 2012 Cristian Patrasciuc. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "base/basic_macros.h"
#include "base/bind.h"
#include "base/method.h"
#include "base/ptr/scoped_ptr.h"
#include "base/threading/task.h"
#include "base/threading/thread.h"
#include "gtest/gtest.h"

using base::ptr::scoped_ptr;

namespace base {
namespace threading {
namespace {

class TaskTest : public ::testing::Test {
 public:
  TaskTest() {}
  ~TaskTest() {}

  virtual void SetUp() {
    task_was_called_ = false;
    callback_was_called_ = false;
    Reset(origin_thread_, new Thread("Origin Thread"));
    origin_thread_->Start();
    Reset(worker_thread_, new Thread("Worker Thread"));
    worker_thread_->Start();
  }

  virtual void TearDown() {
    if (worker_thread_) {
      StopThread(&worker_thread_);
    }
    if (origin_thread_) {
      StopThread(&origin_thread_);
    }
  }

  void task() {
    EXPECT_TRUE(Thread::CurrentlyOn(Get(worker_thread_)));
    task_was_called_ = true;
  }

  void callback() {
    EXPECT_TRUE(Thread::CurrentlyOn(Get(origin_thread_)));
    callback_was_called_ = true;
  }

 protected:
  bool task_was_called() const {
    return task_was_called_;
  }

  bool callback_was_called() const {
    return callback_was_called_;
  }

  scoped_ptr<Thread>& origin_thread() {
    return origin_thread_;
  }

  scoped_ptr<Thread>& worker_thread() {
    return worker_thread_;
  }

  void StopThread(scoped_ptr<Thread>* thread) {
    (*thread)->SubmitQuitTaskAndJoin();
    Reset(*thread);
  }

 private:
  bool task_was_called_;
  bool callback_was_called_;
  scoped_ptr<Thread> origin_thread_;
  scoped_ptr<Thread> worker_thread_;
};

TEST_F(TaskTest, SimpleTaskOnMainThread) {
  // Force |worker_thread_| to be NULL so we can call task() on the main thread
  StopThread(&worker_thread());
  Task task(FROM_HERE,
            Bind(new Method<void(TaskTest::*)(void)>(&TaskTest::task), this));
  task.Run();
  EXPECT_TRUE(task_was_called());
}

TEST_F(TaskTest, SimpleTask) {
  worker_thread()->SubmitTask(FROM_HERE,
      Bind(new Method<void(TaskTest::*)(void)>(&TaskTest::task), this));
  StopThread(&worker_thread());
  EXPECT_TRUE(task_was_called());
}

TEST_F(TaskTest, TaskWithCallback) {
  Location loc(FROM_HERE);
  Location fake_loc(loc.function(), loc.file_name(), loc.line_number(),
      Get(origin_thread()));
  worker_thread()->SubmitTask(fake_loc,
      Bind(new Method<void(TaskTest::*)(void)>(&TaskTest::task), this),
      Bind(new Method<void(TaskTest::*)(void)>(&TaskTest::callback), this));
  StopThread(&worker_thread());
  EXPECT_TRUE(task_was_called());
  StopThread(&origin_thread());
  EXPECT_TRUE(callback_was_called());
}

TEST_F(TaskTest, DEBUG_ONLY_TEST(PostTaskWithCallbackFromMainThreadDeathTest)) {
  ::testing::FLAGS_gtest_death_test_style = "threadsafe";
  ASSERT_DEATH(
      {  // NOLINT(whitespace/braces)
        worker_thread()->SubmitTask(FROM_HERE,
            Bind(new Method<void(TaskTest::*)(void)>(&TaskTest::task), this),
            Bind(new Method<void(TaskTest::*)(void)>(&TaskTest::task), this));
        StopThread(&worker_thread());
      },
      "callbacks from the main thread");
}

}  // anonymous namespace
}  // threading namespace
}  // base namespace
