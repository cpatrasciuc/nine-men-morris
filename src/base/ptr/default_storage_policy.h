// Copyright (c) 2012 Cristian Patrasciuc. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef BASE_PTR_DEFAULT_STORAGE_POLICY_H_
#define BASE_PTR_DEFAULT_STORAGE_POLICY_H_

#include "base/basic_macros.h"

namespace base {
namespace ptr {

template <class T>
class DefaultStoragePolicy {
 public:
  /* The type of the private stored variable (pointee__) */
  typedef T* StoredType;

  /* The type returned by operator-> */
  typedef T* PointerType;

  /* The type returned by operator* */
  typedef T& ReferenceType;

 public:
  DefaultStoragePolicy() : pointee_(0) {}
  explicit DefaultStoragePolicy(const StoredType& p): pointee_(p) {}

  /* Smart pointer behavior */
  PointerType operator->() const {
    // TODO(macros): add debug check for non-null pointer
    return pointee_;
  }

  ReferenceType operator*() const {
    // TODO(macros): add debug check for non-null pointer
    return *pointee_;
  }

  /* These functions provide explicit access to the encapsulated pointer */
  friend inline PointerType Get(const DefaultStoragePolicy& sp) {
    return sp.pointee_;
  }

  friend inline const StoredType& GetImplAsRef(const DefaultStoragePolicy& sp) {
    return sp.pointee_;
  }

  friend inline StoredType& GetImplAsRef(DefaultStoragePolicy& sp) {  // NOLINT
    return sp.pointee_;
  }

 protected:
  /* This function is responsible for deleting/deallocating
   * the encapsulated pointer. Can be overwritten by subclasses. */
  void Destroy() {
    delete pointee_;
  }

 private:
  StoredType pointee_;

  DISALLOW_COPY_AND_ASSIGN(DefaultStoragePolicy);
};

}  // namespace ptr
}  // namespace base

#endif  // BASE_PTR_DEFAULT_STORAGE_POLICY_H_

