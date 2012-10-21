// Copyright (c) 2012 Cristian Patrasciuc. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "base/threading/condition_variable.h"

#include <pthread.h>
#include <errno.h>

#include "base/log.h"
#include "base/ptr/scoped_ptr.h"
#include "base/threading/lock.h"

namespace base {
namespace threading {

ConditionVariable::ConditionVariable(Lock* lock)
    : lock_(lock) {
  int result = pthread_cond_init(&cond_, NULL);
  if (result) {
    ELOG(ERROR) << "Error while initializing condition variable: ";
  }
}

ConditionVariable::~ConditionVariable() {
  int result = pthread_cond_destroy(&cond_);
  if (result) {
    ELOG(ERROR) << "Error while destroying condition variable: ";
  }
}

void ConditionVariable::Wait() {
  DCHECK(!lock_->TryAcquire());
  MutexLockImpl* mutex = static_cast<MutexLockImpl*>(Get(lock_->lock_impl_));
  DCHECK(mutex != NULL);
  int result = pthread_cond_wait(&cond_, &mutex->mutex_);
  if (result) {
    DCHECK(result != EPERM) << "Invalid lock provided";
    ELOG(ERROR) << "Error while waiting on condition variable: " << result;
  }
}

bool ConditionVariable::TimedWait(timespec delta) {
  DCHECK(!lock_->TryAcquire());
  MutexLockImpl* mutex = static_cast<MutexLockImpl*>(Get(lock_->lock_impl_));
  DCHECK(mutex);
  int result = pthread_cond_timedwait(&cond_, &mutex->mutex_, &delta);
  if (!(result == 0 || result == ETIMEDOUT)) {
    DCHECK(result != EPERM) << "Invalid lock provided";
    ELOG(ERROR) << "Error while waiting on condition variable: ";
  }
  return result == 0;
}

void ConditionVariable::Signal() {
  DCHECK(!lock_->TryAcquire());
  int result = pthread_cond_signal(&cond_);
  if (result) {
    ELOG(ERROR) << "Error while signaling condition variable: ";
  }
}

void ConditionVariable::Broadcast() {
  DCHECK(!lock_->TryAcquire());
  int result = pthread_cond_broadcast(&cond_);
  if (result) {
    ELOG(ERROR) << "Error while broadcasting condition variable: ";
  }
}

}  // namespace threading
}  // namespace base


