// Copyright (c) 2012 Cristian Patrasciuc. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef BASE_PTR_REF_PTR_H_
#define BASE_PTR_REF_PTR_H_

#include "base/ptr/default_storage_policy.h"
#include "base/ptr/ref_counted_ownership_policy.h"
#include "base/ptr/ref_counted_storage_policy.h"
#include "base/ptr/smart_ptr.h"

namespace base {
namespace ptr {

template <class T>
class ref_ptr : public SmartPtr<T,
                                RefCountedOwnershipPolicy,
                                RefCountedStoragePolicy> {
  typedef typename SmartPtr<T,
                            RefCountedOwnershipPolicy,
                            RefCountedStoragePolicy>::StoredType StoredType;
  typedef typename SmartPtr<T,
                            RefCountedOwnershipPolicy,
                            RefCountedStoragePolicy>::PointerType PointerType;
 public:
  ref_ptr()
      : SmartPtr<T, RefCountedOwnershipPolicy, RefCountedStoragePolicy>() {}
  explicit ref_ptr(const StoredType& other)
      : SmartPtr<T, RefCountedOwnershipPolicy, RefCountedStoragePolicy>(other) {
  }

  friend PointerType Release(ref_ptr<T>& ptr) {  // NOLINT(runtime/references)
    PointerType result = Get(ptr);
    GetImplAsRef(ptr) = NULL;
    return result;
  }
};

}  // namespace ptr
}  // namespace base

#endif  // BASE_PTR_REF_PTR_H_

