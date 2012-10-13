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
  static void* Get(Key* key);
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

  T* Get() {
    return static_cast<T*>(ThreadSpecificImpl::Get(&key_));
  }

  void Set(T* value) {
    ThreadSpecificImpl::Set(&key_, value);
  }

 private:
  typename ThreadSpecificImpl::Key key_;

  DISALLOW_COPY_AND_ASSIGN(ThreadSpecific);
};

}  // namespace threading
}  // namespace base

#endif  // BASE_THREADING_THREAD_SPECIFIC_H_

