// Copyright (c) 2012 Cristian Patrasciuc. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef BASE_PTR_SUPPORTS_WEAK_REF_H_
#define BASE_PTR_SUPPORTS_WEAK_REF_H_

#include <algorithm>
#include <deque>

#include "base/log.h"
#include "base/ptr/default_storage_policy.h"
#include "base/ptr/ref_counted.h"
#include "base/ptr/weak_ownership_policy.h"
#include "base/ptr/weak_ptr.h"

namespace base {
namespace ptr {

// TODO(visibility): Make the public methods available only to
// WeakOwnershipPolicy

// TODO(smart_pointers): Add a WeakRefFactory

template <class T>
class SupportsWeakRef {
 public:
  typedef typename DefaultStoragePolicy<T>::PointerType PointerType;
  typedef typename WeakOwnershipPolicy<PointerType>::Observer ObserverType;

  ~SupportsWeakRef() {
    InvalidateWeakRefs();
  }

  void AddWeakRef(const ObserverType* refferer) const {
    DCHECK_EQ(std::find(weak_refs_.begin(), weak_refs_.end(), refferer),
              weak_refs_.end()) << "Too many calls to AddWeakRef()";
    weak_refs_.push_back(refferer);
  }

  void ReleaseWeakRef(const ObserverType* refferer) const {
    DCHECK_GT(weak_refs_.size(), 0) << "Too many calls to ReleaseWeakRef()";
    typename std::deque<const ObserverType*>::iterator it;
    it = std::find(weak_refs_.begin(), weak_refs_.end(), refferer);
    DCHECK(it != weak_refs_.end()) << "Trying to remove an inexistent ref";
    weak_refs_.erase(it);
  }

  void InvalidateWeakRefs() const {
    std::for_each(weak_refs_.begin(), weak_refs_.end(), Invalidate);
  }

 private:
  static void Invalidate(const ObserverType* p) {
    p->set_valid(false);
  }

  mutable std::deque<const ObserverType*> weak_refs_;
};

}  // namespace ptr
}  // namespace base

#endif  // BASE_PTR_SUPPORTS_WEAK_REF_H_

