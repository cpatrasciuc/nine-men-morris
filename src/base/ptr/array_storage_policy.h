// Copyright (c) 2012 Cristian Patrasciuc. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef BASE_PTR_ARRAY_STORAGE_POLICY_H_
#define BASE_PTR_ARRAY_STORAGE_POLICY_H_

#include "base/ptr/default_storage_policy.h"

namespace base {
namespace ptr {

template<class T>
class ArrayStoragePolicy : public DefaultStoragePolicy<T> {
 protected:
  typedef typename DefaultStoragePolicy<T>::StoredType StoredType;

 public:
  ArrayStoragePolicy() : DefaultStoragePolicy<T>() {}
  explicit ArrayStoragePolicy(const StoredType& t)
      : DefaultStoragePolicy<T>(t) {}

  T& operator[](size_t index) {
    return Get(*this)[index];
  }

 protected:
  /* Override the default implementation to use the delete[] operator */
  void Destroy() {
    typename DefaultStoragePolicy<T>::StoredType& pointee = GetImplAsRef(*this);
    delete[] pointee;
  }
};

}  // namespace ptr
}  // namespace base

#endif  // BASE_PTR_ARRAY_STORAGE_POLICY_H_

