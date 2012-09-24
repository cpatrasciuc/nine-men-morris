// Copyright (c) 2012 Cristian Patrasciuc. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef BASE_PTR_MALLOC_STORAGE_POLICY_H_
#define BASE_PTR_MALLOC_STORAGE_POLICY_H_

#include <cstdlib>

#include "base/ptr/default_storage_policy.h"

namespace base {
namespace ptr {

template<class T>
class MallocStoragePolicy : public DefaultStoragePolicy<T> {
 protected:
  typedef typename DefaultStoragePolicy<T>::StoredType StoredType;

 public:
  MallocStoragePolicy() : DefaultStoragePolicy<T>() {}
  explicit MallocStoragePolicy(const StoredType& p)
      : DefaultStoragePolicy<T>(p) {}

  T& operator[](size_t index) {
    return Get(*this)[index];
  }

 protected:
  void Destroy() {
    typename DefaultStoragePolicy<T>::StoredType& pointee = GetImplAsRef(*this);
    std::free(const_cast<void*>(reinterpret_cast<const void*>(pointee)));
    pointee = 0;
  }
};

}  // namespace ptr
}  // namespace base

#endif  // BASE_PTR_MALLOC_STORAGE_POLICY_H_

