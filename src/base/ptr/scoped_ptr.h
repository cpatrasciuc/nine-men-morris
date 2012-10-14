// Copyright (c) 2012 Cristian Patrasciuc. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef BASE_PTR_SCOPED_PTR_H_
#define BASE_PTR_SCOPED_PTR_H_

#include "base/basic_macros.h"
#include "base/ptr/scoped_ownership_policy.h"
#include "base/ptr/smart_ptr.h"

namespace base {
namespace ptr {

template <typename T>
class scoped_ptr : public SmartPtr<T, ScopedOwnershipPolicy> {
 private:
  typedef typename SmartPtr<T, ScopedOwnershipPolicy>::StoredType StoredType;
  typedef typename SmartPtr<T, ScopedOwnershipPolicy>::PointerType PointerType;
 public:
  scoped_ptr() : SmartPtr<T, ScopedOwnershipPolicy>() {}
  explicit scoped_ptr(const StoredType& t) :
      SmartPtr<T, ScopedOwnershipPolicy>(t) {}

  friend void Reset(scoped_ptr<T>& smart_ptr, const StoredType& raw_ptr = 0) {
    scoped_ptr<T> new_ptr(raw_ptr);
    new_ptr.Swap(smart_ptr);
  };

  friend PointerType Release(scoped_ptr<T>* ptr) {
    PointerType result = Get(*ptr);
    GetImplAsRef(*ptr) = NULL;
    return result;
  }

 private:
  DISALLOW_COPY_AND_ASSIGN(scoped_ptr<T>);
};

}  // namespace ptr
}  // namespace base

#endif  // BASE_PTR_SCOPED_PTR_H_

