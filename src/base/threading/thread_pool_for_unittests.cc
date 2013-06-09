// Copyright (c) 2012 Cristian Patrasciuc. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "base/threading/thread_pool_for_unittests.h"

#include "base/callable.h"
#include "base/log.h"
#include "base/threading/thread.h"
#include "base/string_util.h"

// TODO(threading): Provide a real thread poll with shared work queue and revert
//                  this to the testing version.

namespace base {
namespace threading {

const int kDefaultThreadCount = 10;

ThreadPoolForUnittests::ThreadPoolForUnittests(int thread_count)
    : thread_count_(thread_count), threads_() {}

void ThreadPoolForUnittests::CreateThreads() {
  DCHECK_GT(thread_count_, 0);
  DCHECK_EQ(threads_.size(), 0U);
  for (int i = 0; i < thread_count_; ++i) {
    threads_.push_back(new Thread("Test Thread " + ToString(i+1)));
  }
}

void ThreadPoolForUnittests::StartThreads() {
  DCHECK_EQ(thread_count_, static_cast<int>(threads_.size()))
      << "CreateThreads() was not called";
  for (size_t i = 0; i < threads_.size(); ++i) {
    DCHECK(!threads_[i]->is_running());
    threads_[i]->Start();
    DCHECK(threads_[i]->is_running());
    DCHECK(!Thread::CurrentlyOn(threads_[i]));
  }
}

void ThreadPoolForUnittests::StopAndJoinThreads() {
  for (size_t i = 0; i < threads_.size(); ++i) {
    DCHECK(threads_[i]->is_running())
        << "Thread '" << threads_[i]->name() << "' is already stopped";
    threads_[i]->SubmitQuitTaskAndJoin();
    delete threads_[i];
  }
}

void ThreadPoolForUnittests::SubmitTask(int thread_number,
                                Location loc,
                                Closure* closure,
                                Closure* callback) {
  DCHECK_LT(thread_number, thread_count());
  DCHECK_EQ(static_cast<int>(threads_.size()), thread_count());
  threads_[thread_number]->SubmitTask(loc, closure, callback);
}

}  // namespace threading
}  // namespace base


