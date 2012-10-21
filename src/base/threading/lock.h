// Copyright (c) 2012 Cristian Patrasciuc. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef BASE_THREADING_LOCK_H_
#define BASE_THREADING_LOCK_H_

#include <pthread.h>

#include "base/base_export.h"
#include "base/basic_macros.h"
#include "base/ptr/scoped_ptr.h"

namespace base {
namespace threading {

class ConditionVariable;

class BASE_EXPORT LockImpl {
 public:
  virtual ~LockImpl() {}
  virtual void Acquire() = 0;
  virtual void Release() = 0;

  // Tries to acuire the lock without blocking. If it is already acquired by
  // someone else, it returns |false|; otherwise it returns |true|.
  virtual bool TryAcquire() = 0;
};

class BASE_EXPORT MutexLockImpl : public LockImpl {
 public:
  MutexLockImpl();
  virtual ~MutexLockImpl();

  virtual void Acquire()    { pthread_mutex_lock(&mutex_); }
  virtual void Release()    { pthread_mutex_unlock(&mutex_); }
  virtual bool TryAcquire() { return !pthread_mutex_trylock(&mutex_); }

 private:
  pthread_mutex_t mutex_;

  friend class ConditionVariable;
  DISALLOW_COPY_AND_ASSIGN(MutexLockImpl);
};

class BASE_EXPORT SpinLockImpl : public LockImpl {
 public:
  SpinLockImpl();
  virtual ~SpinLockImpl();

  virtual void Acquire()    { pthread_spin_lock(&spinlock_); }
  virtual void Release()    { pthread_spin_unlock(&spinlock_); }
  virtual bool TryAcquire() { return !pthread_spin_trylock(&spinlock_); }

 private:
  pthread_spinlock_t spinlock_;

  DISALLOW_COPY_AND_ASSIGN(SpinLockImpl);
};

class BASE_EXPORT Lock {
 public:
  explicit Lock(LockImpl* lock_impl = new MutexLockImpl);

  void Acquire()    { lock_impl_->Acquire();           }
  void Release()    { lock_impl_->Release();           }
  bool TryAcquire() { return lock_impl_->TryAcquire(); }

 private:
  base::ptr::scoped_ptr<LockImpl> lock_impl_;

  friend class ConditionVariable;
  DISALLOW_COPY_AND_ASSIGN(Lock);
};

}  // namespace threading
}  // namespace base

#endif  // BASE_THREADING_LOCK_H_
