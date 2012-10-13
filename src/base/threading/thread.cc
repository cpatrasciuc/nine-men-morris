// Copyright (c) 2012 Cristian Patrasciuc. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "base/log.h"
#include "base/threading/thread.h"

namespace base {
namespace threading {

Thread::Thread(std::string name)
    : name_(name), thread_id(-1), is_running_(false) {}

Thread::~Thread() {}

bool Thread::Start() {
  // TODO(threading): Add thread options
  if (!pthread_create(&thread_id, NULL, &Thread::StartThreadThunk, this)) {
    return true;
  }
  thread_id = -1;
  return false;
}

void Thread::Join() {
  pthread_join(thread_id, NULL);
}

void* Thread::StartThreadThunk(void* thread) {
  Thread* current_thread = reinterpret_cast<Thread*>(thread);
  current_thread->RunInternal();
  return NULL;  // Not used
}

void Thread::RunInternal() {
  DCHECK(thread_id == pthread_self());
  Thread::current_thread.Set(this);
  is_running_ = true;
  LOG(DEBUG) << name() << "(" << thread_id << ")";
}

// static
const Thread* Thread::Current() {
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
