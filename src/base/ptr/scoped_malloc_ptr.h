// Copyright (c) 2012 Cristian Patrasciuc. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef BASE_PTR_SCOPED_MALLOC_PTR_H_
#define BASE_PTR_SCOPED_MALLOC_PTR_H_

#include "base/basic_macros.h"
#include "base/ptr/malloc_storage_policy.h"
#include "base/ptr/scoped_ownership_policy.h"
#include "base/ptr/smart_ptr.h"

namespace base {
namespace ptr {

template <typename T>
class scoped_malloc_ptr : public SmartPtr<T,
                                          ScopedOwnershipPolicy,
                                          MallocStoragePolicy> {
 private:
  typedef typename SmartPtr<T,
                            ScopedOwnershipPolicy,
                            MallocStoragePolicy>::StoredType StoredType;
 public:
  scoped_malloc_ptr()
      : SmartPtr<T, ScopedOwnershipPolicy, MallocStoragePolicy>() {}
  explicit scoped_malloc_ptr(const StoredType& t) :
      SmartPtr<T, ScopedOwnershipPolicy, MallocStoragePolicy>(t) {}

  friend void Reset(scoped_malloc_ptr<T>& p, const StoredType& raw_ptr) {
    scoped_malloc_ptr<T> new_ptr(raw_ptr);
    new_ptr.Swap(p);
  };

 private:
  DISALLOW_COPY_AND_ASSIGN(scoped_malloc_ptr<T>);
};

}  // namespace ptr
}  // namespace base

#endif  // BASE_PTR_SCOPED_MALLOC_PTR_H_

