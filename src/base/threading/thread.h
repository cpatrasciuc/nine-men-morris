// Copyright (c) 2012 Cristian Patrasciuc. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef BASE_THREADING_THREAD_H_
#define BASE_THREADING_THREAD_H_

#include <pthread.h>

#include <deque>
#include <string>

#include "base/base_export.h"
#include "base/basic_macros.h"
#include "base/callable.h"
#include "base/location.h"
#include "base/threading/lock.h"
#include "base/threading/thread_specific.h"

namespace base {
namespace threading {

class Task;

typedef pthread_t ThreadID;

class BASE_EXPORT Thread {
 public:
  explicit Thread(std::string name);
  ~Thread();

  const std::string& name() const {
    return name_;
  }

  bool is_running() const {
    return is_running_;
  }

  // Starts |this| thread. Return |true| if the new thread was started
  // successfully; |false| otherwise.
  bool Start();

  // Blocks until |this| thread is over.
  void Join();

  // This method can be called from any thread. It submits a new task to this
  // thread. It's an asynchronous method. It returns without waiting for the
  // thread to actually run the new task. This thread will notify the submitter
  // thread by posting the |callback| to its task queue once this task is run,
  // if |callback| is not NULL.
  void SubmitTask(Location location,
                  Closure* closure,
                  Closure* callback = NULL);

  // Post a task on this thread that will make it quit as soon as it reaches the
  // idle state.
  void QuitWhenIdle();

  // Utility method that calls QuitWhenIdle() and then immediately calls Join()
  void SubmitQuitTaskAndJoin();

  // Returns a pointer to the current Thread object
  static Thread* Current();

  // Check if the method is called on the thread represented by |t|.
  static bool CurrentlyOn(Thread* t);

 private:
  // Static method passed to pthread_create when starting a new thread. The
  // argument is a pointer to the current thread object. It just calls the
  // RunLoop() method on it.
  static void* StartThreadThunk(void* thread);

  // Helper function to be placed at the end of the task queue and set the
  // |quit_when_idle_| member to false.
  void QuitInternal();

  // Stores a pointer to the Thread object corresponding to the current thread
  static ThreadSpecific<Thread> current_thread;

  void Run();

  std::string name_;

  ThreadID thread_id;

  bool is_running_;

  bool quit_when_idle_;

  bool was_joined_;

  // This queue is accessible from any other thread through SubmitTask* methods
  std::deque<Task*> public_queue_;

  // The lock used to synchronize access to the public task queue
  base::threading::Lock public_queue_lock_;

  // The internal task queue. From time to time all the tasks from the public
  // queue are moved into the internal queue. This is not accessible outside
  // of the thread object. Is used to buffer incoming work and avoid locking the
  // public queue while executing it.
  std::deque<Task*> internal_queue_;

  DISALLOW_COPY_AND_ASSIGN(Thread);
};

}  // namespace threading
}  // namespace base

#endif  // BASE_THREADING_THREAD_H_

