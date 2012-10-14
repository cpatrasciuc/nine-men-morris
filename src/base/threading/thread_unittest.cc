// Copyright (c) 2012 Cristian Patrasciuc. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include <vector>

#include "base/bind.h"
#include "base/location.h"
#include "base/method.h"
#include "base/string_util.h"
#include "base/threading/lock.h"
#include "base/threading/thread.h"
#include "gtest/gtest.h"

namespace base {
namespace threading {
namespace {

class Counter {
 public:
  Counter() : counter_(0) {}

  void IncrementCounter() {
    ScopedGuard _(&counter_lock_);
    ++counter_;
  }

  int counter() const {
    return counter_;
  }

 private:
  // TODO(threading) : Replace this with atomic variable
  int counter_;
  Lock counter_lock_;
};

TEST(Thread, Basic) {
  int n = 10;
  Counter c;
  std::vector<Thread*> threads;

  for (int i = 0; i < n; ++i) {
    threads.push_back(new Thread("Test Thread " + ToString(i+1)));
  }

  for (size_t i = 0; i < threads.size(); ++i) {
    EXPECT_FALSE(threads[i]->is_running());
    threads[i]->Start();
    EXPECT_TRUE(threads[i]->is_running());
    EXPECT_FALSE(Thread::CurrentlyOn(threads[i]));
  }

  for (size_t i = 0; i < threads.size(); ++i) {
    threads[i]->SubmitTask(FROM_HERE,
      Bind(new Method<void(Counter::*)(void)>(&Counter::IncrementCounter), &c));
  }

  for (size_t i = 0; i < threads.size(); ++i) {
    threads[i]->QuitWhenIdle();
  }

  for (size_t i = 0; i < threads.size(); ++i) {
    threads[i]->Join();
    delete threads[i];
  }

  EXPECT_EQ(n, c.counter());
}

}  // anonymous namespace
}  // namespace threading
}  // namespace base
