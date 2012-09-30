// Copyright (c) 2012 Cristian Patrasciuc. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef BASE_PTR_WEAK_PTR_H_
#define BASE_PTR_WEAK_PTR_H_

#include "base/ptr/ref_ptr.h"
#include "base/ptr/smart_ptr.h"
#include "base/ptr/supports_weak_ref.h"
#include "base/ptr/weak_ownership_policy.h"

namespace base {
namespace ptr {

template <class T>
class weak_ptr : public SmartPtr<T, WeakOwnershipPolicy> {
 protected:
  typedef typename SmartPtr<T, WeakOwnershipPolicy>::StoredType StoredType;
  typedef typename SmartPtr<T, WeakOwnershipPolicy>::PointerType PointerType;

 public:
  weak_ptr() : SmartPtr<T, WeakOwnershipPolicy>() {}

  weak_ptr(const StoredType& t)  // NOLINT(runtime/explicit)
      : SmartPtr<T, WeakOwnershipPolicy>(t) {}

  weak_ptr(const weak_ptr& other)
      : SmartPtr<T, WeakOwnershipPolicy>(other) {}

  template <class T2>
  weak_ptr(const weak_ptr<T2>& other)
      : SmartPtr<T, WeakOwnershipPolicy>(other) {}

  friend inline PointerType Get(const weak_ptr& wp) {
    return (wp.IsValid() ?
      Get(static_cast<typename weak_ptr::StoragePolicyType>(wp)) : NULL);
  }

  friend inline ref_ptr<T> Lock(const weak_ptr& wp) {
    return ref_ptr<T>(wp.IsValid() ? Get(wp) : NULL);
  }
};

}  // namespace ptr
}  // namespace base

#endif  // BASE_PTR_WEAK_PTR_H_

