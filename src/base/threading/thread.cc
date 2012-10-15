// Copyright (c) 2012 Cristian Patrasciuc. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "base/log.h"
#include "base/bind.h"
#include "base/method.h"
#include "base/ptr/scoped_ptr.h"
#include "base/threading/task.h"
#include "base/threading/thread.h"

namespace base {
namespace threading {

Thread::Thread(std::string name)
    : name_(name),
      thread_id(-1),
      is_running_(false),
      quit_when_idle_(false),
      public_queue_(),
      public_queue_lock_(),
      internal_queue_() {
}

Thread::~Thread() {}

bool Thread::Start() {
  // TODO(threading): Add thread options
  if (!pthread_create(&thread_id, NULL, &Thread::StartThreadThunk, this)) {
    is_running_ = true;
    return true;
  }
  thread_id = -1;
  return false;
}

void Thread::Join() {
  pthread_join(thread_id, NULL);
}

void Thread::SubmitTask(Location location,
                        Closure* closure,
                        Closure* callback) {
  ScopedGuard _(&public_queue_lock_);
  public_queue_.push_back(new Task(location, closure, callback));
}

void Thread::QuitWhenIdle() {
  Closure* quit_closure =
    Bind(new Method<void(Thread::*)(void)>(&Thread::QuitInternal), this);
  SubmitTask(FROM_HERE, quit_closure);
}

void Thread::QuitInternal() {
  DCHECK(Thread::CurrentlyOn(this));
  // TODO(threading): Possible problem if others submit to the working queue?
  quit_when_idle_ = true;
}

void Thread::SubmitQuitTaskAndJoin() {
  QuitWhenIdle();
  Join();
}

void* Thread::StartThreadThunk(void* thread) {
  Thread* current_thread = static_cast<Thread*>(thread);
  current_thread->RunInternal();
  return NULL;  // Not used
}

void Thread::RunInternal() {
  DCHECK(thread_id == pthread_self());
  Thread::current_thread.Set(this);

  while (true) {
    bool did_some_work = false;

    if (!internal_queue_.empty()) {
      did_some_work = true;
      base::ptr::scoped_ptr<Task> next_task(internal_queue_.front());
      internal_queue_.pop_front();
      next_task->Run();
    } else if (!quit_when_idle_) {
      ScopedGuard _(&public_queue_lock_);
      if (!public_queue_.empty()) {
        did_some_work = true;
        internal_queue_.swap(public_queue_);
      }
    }

    if (!did_some_work && quit_when_idle_) {
      break;
    }
  }
}

// static
Thread* Thread::Current() {
  return Thread::current_thread.Get();
}

// static
bool Thread::CurrentlyOn(Thread* t) {
  return t == Thread::Current();
}

// static
ThreadSpecific<Thread> Thread::current_thread;

}  // namespace threading
}  // namespace base
