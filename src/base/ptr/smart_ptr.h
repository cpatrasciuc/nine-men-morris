// Copyright (c) 2012 Cristian Patrasciuc. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef BASE_PTR_SMART_PTR_H_
#define BASE_PTR_SMART_PTR_H_

#include "base/ptr/default_storage_policy.h"

namespace base {
namespace ptr {

template <
  typename T,
  template <class> class OwnershipPolicy = DefaultStoragePolicy  // NOLINT
>
class SmartPtr : public OwnershipPolicy<T> {
 public:
  typedef typename OwnershipPolicy<T>::PointerType PointerType;
  typedef typename OwnershipPolicy<T>::StoredType StoredType;
  typedef typename OwnershipPolicy<T>::ReferenceType ReferenceType;

  SmartPtr() : OwnershipPolicy<T>() {}
  explicit SmartPtr(const StoredType& t) : OwnershipPolicy<T>(t) {}

  SmartPtr(const SmartPtr<T>& other) {
    // TODO(smart_pointer): Implement the copy constructor
  }

  ~SmartPtr() {
    // OwnershipPolicy<T>::Destroy();
  }

  // This enables statements like: if (!smart_ptr) { ... }
  bool operator!() const {
    return Get(*this) == 0;
  }

  // Equality and inequality operators (non-template versions)
  inline friend bool operator==(const SmartPtr& lhs, const PointerType rhs) {
    return Get(lhs) == rhs;
  }

  inline friend bool operator==(const PointerType lhs, const SmartPtr& rhs) {
    return lhs == Get(rhs);
  }

  inline friend bool operator!=(const SmartPtr& lhs, const PointerType rhs) {
    return Get(lhs) != rhs;
  }

  inline friend bool operator!=(const PointerType lhs, const SmartPtr& rhs) {
    return lhs != Get(rhs);
  }

  // Equality and inequality operators (template versions)
  template <class U>
  inline friend bool operator==(const SmartPtr& lhs, const U* rhs) {
    return Get(lhs) == rhs;
  }

  template <class U>
  inline friend bool operator==(const U* lhs, const SmartPtr& rhs) {
    return lhs == Get(rhs);
  }

  template <class U>
  inline friend bool operator!=(const SmartPtr& lhs, const U* rhs) {
    return Get(lhs) != rhs;
  }

  template <class U>
  inline friend bool operator!=(const U* lhs, const SmartPtr& rhs) {
    return lhs != Get(rhs);
  }

  // Equality and inequality operators for SmartPtr itself;
  // delegates to a comparison between T* and U*.
  template <class U>
  bool operator==(const SmartPtr<U>& other) const {
    return Get(*this) == Get(other);
  }

  template <class U>
  bool operator!=(const SmartPtr<U>& other) const {
    return Get(*this) != Get(other);
  }
};

}  // namespace ptr
}  // namespace base

#endif  // BASE_PTR_SMART_PTR_H_

