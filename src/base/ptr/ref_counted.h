// Copyright (c) 2012 Cristian Patrasciuc. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef BASE_PTR_REF_COUNTED_H_
#define BASE_PTR_REF_COUNTED_H_

#include "base/base_export.h"

namespace base {
namespace ptr {

// TODO(smart_pointers) : Make this a class template
// TODO(visibility): Make the public methods visible only to
//                   RefCountedOwnershipPolicy
class BASE_EXPORT RefCounted {
 public:
  void AddRef() const;
  bool Release() const;

  const bool HasOnlyOneRef() const {
    return ref_count_ == 1;
  }

 protected:
  RefCounted();
  ~RefCounted();

 private:
  mutable int ref_count_;
};

}  // namespace ptr
}  // namespace base

#endif  // BASE_PTR_REF_COUNTED_H_

