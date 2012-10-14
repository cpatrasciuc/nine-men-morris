// Copyright (c) 2012 Cristian Patrasciuc. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "base/threading/thread_specific.h"

#include "base/log.h"

namespace base {
namespace threading {

void ThreadSpecificImpl::New(Key* key) {
  int result = pthread_key_create(key, NULL);  // No special destructor
  if (result) {
    ELOG(ERROR) << "Could not create shared key: ";
  }
}

void ThreadSpecificImpl::Delete(Key* key) {
  if (!ThreadSpecificImpl::Get(key)) {
    return;
  }
  int result = pthread_key_delete(*key);
  if (result) {
    ELOG(ERROR) << "Could not delete shared key: ";
  }
}

void* ThreadSpecificImpl::Get(const Key* key) {
  return pthread_getspecific(*key);
}

void ThreadSpecificImpl::Set(Key* key, const void* value) {
  int result = pthread_setspecific(*key, value);
  if (result) {
    ELOG(ERROR) << "Could not set specific key: ";
  }
}

}  // namespace threading
}  // namespace base
