// Copyright (c) 2012 Cristian Patrasciuc. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef BASE_THREADING_CONDITION_VARIABLE_H_
#define BASE_THREADING_CONDITION_VARIABLE_H_

#include <pthread.h>

#include <ctime>

#include "base/base_export.h"
#include "base/basic_macros.h"

namespace base {
namespace threading {

class Lock;

class BASE_EXPORT ConditionVariable {
 public:
  // Initializes a new condition variable by 'binding' it to the specified Lock
  // object. All the other methods of this condition variable must be called
  // while the |lock| is acquired.
  explicit ConditionVariable(Lock* lock);

  // Destroys this condition variable. It does not delete the |lock| object
  // associated with it.
  ~ConditionVariable();

  // Blocks the current thread and waits until another thread calls Signal() or
  // Broadcast() on this condition variable.
  void Wait();

  // Similar with Wait() but supports a timeout interval specified by |delta|.
  // If the timeout was exceeded and the condition was not signaled, this
  // method returns |false|, else it returns |true|.
  bool TimedWait(timespec delta);

  // Unblocks one thread waiting on this condition variable.
  void Signal();

  // Unblocks all threads waiting on this condition variable.
  void Broadcast();

  Lock* lock() const { return lock_; }

 private:
  Lock* lock_;
  pthread_cond_t cond_;

  DISALLOW_COPY_AND_ASSIGN(ConditionVariable);
};

}  // namespace threading
}  // namespace base

#endif  // BASE_THREADING_CONDITION_VARIABLE_H_

