// Copyright (c) 2012 Cristian Patrasciuc. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "base/bind.h"
#include "base/method.h"
#include "base/ptr/scoped_ptr.h"
#include "base/threading/task.h"
#include "gtest/gtest.h"

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
  }

 protected:
  bool task_was_called() const {
    return task_was_called_;
  }

  bool callback_was_called() const {
    return callback_was_called_;
  }

  void task() {
    task_was_called_ = true;
  }

  void callback() {
    callback_was_called_ = true;
  }

 private:
  bool task_was_called_;
  bool callback_was_called_;
};

TEST_F(TaskTest, SimpleTask) {
  Task task(FROM_HERE,
            Bind(new Method<void(TaskTest::*)(void)>(&TaskTest::task), this));
  task.Run();
  EXPECT_TRUE(task_was_called());
}

TEST_F(TaskTest, TaskWithCallback) {
  Task task(FROM_HERE,
      Bind(new Method<void(TaskTest::*)(void)>(&TaskTest::task), this),
      Bind(new Method<void(TaskTest::*)(void)>(&TaskTest::callback), this));
  task.Run();
  EXPECT_TRUE(task_was_called());
  EXPECT_TRUE(callback_was_called());
}

}  // anonymous namespace
}  // threading namespace
}  // base namespace
