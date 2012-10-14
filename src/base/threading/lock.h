// Copyright (c) 2012 Cristian Patrasciuc. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef BASE_THREADING_LOCK_H_
#define BASE_THREADING_LOCK_H_

#include <pthread.h>

#include "base/base_export.h"

namespace base {
namespace threading {

class BASE_EXPORT Lock {
 public:
  Lock();

  void Acquire() {
    pthread_mutex_lock(&lock_impl_);
  };

  void Release() {
    pthread_mutex_unlock(&lock_impl_);
  };

  bool TryAcquire() {
    return pthread_mutex_trylock(&lock_impl_) == 0;
  };

 private:
  pthread_mutex_t lock_impl_;
};

class BASE_EXPORT ScopedGuard {
 public:
  explicit ScopedGuard(Lock* lock) : lock_(lock) {
    lock_->Acquire();
  }

  ~ScopedGuard() {
    lock_->Release();
  }

 private:
  Lock* lock_;
};

}  // namespace threading
}  // namespace base

#endif  // BASE_THREADING_LOCK_H_

