// Copyright (c) 2012 Cristian Patrasciuc. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef BASE_PTR_SCOPED_ARRAY_PTR_H_
#define BASE_PTR_SCOPED_ARRAY_PTR_H_

#include "base/basic_macros.h"
#include "base/ptr/array_storage_policy.h"
#include "base/ptr/scoped_ownership_policy.h"
#include "base/ptr/smart_ptr.h"

namespace base {
namespace ptr {

template <class T>
class scoped_array_ptr : public SmartPtr<T,
                                         ScopedOwnershipPolicy,
                                         ArrayStoragePolicy> {
 protected:
  typedef typename SmartPtr<T,
                            ScopedOwnershipPolicy,
                            ArrayStoragePolicy>::StoredType StoredType;

 public:
  scoped_array_ptr()
      : SmartPtr<T, ScopedOwnershipPolicy, ArrayStoragePolicy>() {}
  explicit scoped_array_ptr(const StoredType& p)
      : SmartPtr<T, ScopedOwnershipPolicy, ArrayStoragePolicy>(p) {}

  friend void Reset(scoped_array_ptr<T>& ptr, const StoredType& raw_ptr) {
    scoped_array_ptr<T> new_ptr(raw_ptr);
    new_ptr.Swap(ptr);
  }

 private:
  DISALLOW_COPY_AND_ASSIGN(scoped_array_ptr<T>);
};

}  // namespace ptr
}  // namespace base

#endif  // BASE_PTR_SCOPED_ARRAY_PTR_H_

