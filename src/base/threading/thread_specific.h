// Copyright (c) 2012 Cristian Patrasciuc. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef BASE_THREADING_THREAD_SPECIFIC_H_
#define BASE_THREADING_THREAD_SPECIFIC_H_

#include <pthread.h>

#include "base/base_export.h"
#include "base/basic_macros.h"

namespace base {
namespace threading {

class BASE_EXPORT ThreadSpecificImpl {
 public:
  typedef pthread_key_t Key;
  static void New(Key* key);
  static void Delete(Key* key);
  static void* Get(const Key* key);
  static void Set(Key* key, const void* value);
};

template <typename T>
class ThreadSpecific {
 public:
  ThreadSpecific() {
    ThreadSpecificImpl::New(&key_);
  }

  ~ThreadSpecific() {
    ThreadSpecificImpl::Delete(&key_);
  }

  T* Get() const {
    return static_cast<T*>(ThreadSpecificImpl::Get(&key_));
  }

  void Set(T* value) {
    ThreadSpecificImpl::Set(&key_, value);
  }

 private:
  typename ThreadSpecificImpl::Key key_;
};

// TODO(threading): Add a scoped ThreadSpecific class that deallocates the
// object stored at |key_|?

}  // namespace threading
}  // namespace base

#endif  // BASE_THREADING_THREAD_SPECIFIC_H_

