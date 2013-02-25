// Copyright (c) 2012 Cristian Patrasciuc. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef BASE_PTR_REF_COUNTED_H_
#define BASE_PTR_REF_COUNTED_H_

#include "base/base_export.h"
#include "base/log.h"
#include "base/threading/atomic.h"
#include "base/ptr/ref_counted_ownership_policy.h"

namespace base {
namespace ptr {

template <typename T>
class BASE_EXPORT RefCounted {
 public:
  void AddRef() const {
    ++ref_count_;
  }

  bool Release() const {
    DCHECK_GT(ref_count_, 0) << "Too many calls to Release(): " << this;
    --ref_count_;
    return (ref_count_ == 0);
  }

  const bool HasOnlyOneRef() const {
    return ref_count_ == 1;
  }

 protected:
  RefCounted() : ref_count_(0) {}
  ~RefCounted() {
    DCHECK_EQ(ref_count_, 0) << "Manually deleted RefCounted object";
  }

 private:
  mutable int ref_count_;
};

template <typename T>
class BASE_EXPORT RefCountedThreadSafe {
 public:
  void AddRef() const {
    ref_count_.Increment();
  }

  bool Release() const {
    return ref_count_.Decrement() == 0;
  }

  const bool HasOnlyOneRef() const {
    return ref_count_.Get() == 1;
  }

 protected:
  RefCountedThreadSafe() : ref_count_(0) {}
  ~RefCountedThreadSafe() {
    DCHECK_EQ(ref_count_.Get(), 0) << "Manually deleted RefCounted object";
  }

 private:
  mutable base::threading::Atomic<int> ref_count_;
};

}  // namespace ptr
}  // namespace base

#endif  // BASE_PTR_REF_COUNTED_H_

