// Copyright (c) 2012 Cristian Patrasciuc. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include <vector>

#include "base/threading/thread.h"
#include "gtest/gtest.h"

namespace base {
namespace threading {
namespace {

TEST(Thread, Basic) {
  int n = 10;
  std::vector<Thread*> threads;
  for (int i = 0; i < n; ++i) {
    threads.push_back(new Thread("TestThread"));
  }
  for (size_t i = 0; i < threads.size(); ++i) {
    EXPECT_FALSE(threads[i]->is_running());
    threads[i]->Start();
    EXPECT_FALSE(Thread::CurrentlyOn(threads[i]));
  }
  for (size_t i = 0; i < threads.size(); ++i) {
    threads[i]->Join();
    delete threads[i];
  }
}

}  // anonymous namespace
}  // namespace threading
}  // namespace base
