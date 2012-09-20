// Copyright (c) 2012 Cristian Patrasciuc. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef BASE_PTR_SMART_PTR_H_
#define	BASE_PTR_SMART_PTR_H_

#include "base/ptr/default_storage_policy.h"

namespace base {
namespace ptr {

template <
  typename T,
  template <class> class OwnershipPolicy = DefaultStoragePolicy
>
class SmartPtr : public OwnershipPolicy<T> {
 public:
  SmartPtr() : OwnershipPolicy<T>() {}
  SmartPtr(const typename OwnershipPolicy<T>::StoredType& t)
      : OwnershipPolicy<T>(t) {}

  SmartPtr(const SmartPtr<T>& other) {
    // TODO(smart_pointer): Implement the copy constructor
  }

  ~SmartPtr() {
    OwnershipPolicy<T>::Destroy();
  }
};

}  // namespace ptr
}  // namespace base

#endif  // BASE_PTR_SMART_PTR_H_

