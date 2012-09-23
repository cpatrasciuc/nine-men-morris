// Copyright (c) 2012 Cristian Patrasciuc. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef BASE_PTR_REF_COUNTED_STORAGE_POLICY_H_
#define BASE_PTR_REF_COUNTED_STORAGE_POLICY_H_

#include "base/ptr/default_storage_policy.h"

namespace base {
namespace ptr {

template <class T>
class RefCountedStoragePolicy : public DefaultStoragePolicy<T> {
 protected:
  typedef typename DefaultStoragePolicy<T>::StoredType StoredType;
 public:
  RefCountedStoragePolicy() : DefaultStoragePolicy<T>() {}
  explicit RefCountedStoragePolicy(const StoredType& p) :
      DefaultStoragePolicy<T>(p) {}

  void Swap(DefaultStoragePolicy<T>& other) {
    if (Get(*this)) {
      Get(*this)->AddRef();
    }
    DefaultStoragePolicy<T>::Swap(other);
  }
};

}  // namespace ptr
}  // namespace base

#endif  // BASE_PTR_REF_COUNTED_STORAGE_POLICY_H_

