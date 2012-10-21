// Copyright (c) 2012 Cristian Patrasciuc. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef BASE_THREADING_THREAD_POOL_FOR_UNITTESTS_H_
#define BASE_THREADING_THREAD_POOL_FOR_UNITTESTS_H_

#include <vector>

#include "base/basic_macros.h"
#include "base/callable.h"
#include "base/location.h"

namespace base {
namespace threading {

class Thread;

// The default number of threads created by ThreadPoolForUnittests
extern const int kDefaultThreadCount;

// This class should be used in unittests that need to create a set of threads
// and perform similar tasks on each one, in parallel.
class ThreadPoolForUnittests {
 public:
  explicit ThreadPoolForUnittests(int thread_count = kDefaultThreadCount);

  // Create the Thread objects
  void CreateThreads();

  // Calls the Thread::Start() method on each thread. Must be called after the
  // CreateThreads() method was called.
  void StartThreads();

  // Calls Thread::SubmitQuitTaskAndJoin() for each thread and the also deletes
  // the Thread object.
  void StopAndJoinThreads();

  // Posts a new task on the thread having the index |thread_number|
  void SubmitTask(int thread_number,
                  Location loc,
                  Closure* closure,
                  Closure* callback = NULL);

  int thread_count() const {
    return thread_count_;
  }

 private:
  int thread_count_;
  std::vector<Thread*> threads_;

  DISALLOW_COPY_AND_ASSIGN(ThreadPoolForUnittests);
};

}  // namespace threading
}  // namespace bases

#endif  // BASE_THREADING_THREAD_POOL_FOR_UNITTESTS_H_

