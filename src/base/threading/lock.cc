// Copyright (c) 2012 Cristian Patrasciuc. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "base/threading/lock.h"

#include <pthread.h>

#include "base/log.h"

namespace base {
namespace threading {

MutexLockImpl::MutexLockImpl() {
#ifdef DEBUG_MODE
  pthread_mutexattr_t attrs;
  ELOG_IF(ERROR, pthread_mutexattr_init(&attrs))
      << "Could not initialized mutex attributes";
  ELOG_IF(ERROR, pthread_mutexattr_settype(&attrs, PTHREAD_MUTEX_ERRORCHECK))
      << "Could not setup mutex attributes";
  pthread_mutex_init(&mutex_, &attrs);
  ELOG_IF(ERROR, pthread_mutexattr_destroy(&attrs))
      << "Could not destroy mutex attributes";
#else
  pthread_mutex_init(&mutex_, NULL);
#endif
}

MutexLockImpl::~MutexLockImpl() {
  pthread_mutex_destroy(&mutex_);
}

SpinLockImpl::SpinLockImpl() {
  pthread_spin_init(&spinlock_, 0);  // 0 = Not shared between processes
}

SpinLockImpl::~SpinLockImpl() {
  pthread_spin_destroy(&spinlock_);
}

Lock::Lock(LockImpl* lock_impl) : lock_impl_(lock_impl) {}

}  // namespace threading
}  // namespace base
