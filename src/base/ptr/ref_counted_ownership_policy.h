// Copyright (c) 2012 Cristian Patrasciuc. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef BASE_PTR_REF_COUNTED_OWNERSHIP_POLICY_H_
#define BASE_PTR_REF_COUNTED_OWNERSHIP_POLICY_H_

namespace base {
namespace ptr {

template <class T>
class RefCountedOwnershipPolicy {
 public:
  RefCountedOwnershipPolicy() {}
  explicit RefCountedOwnershipPolicy(const T& t) {
    if (t) {
      t->AddRef();
    }
  }
  ~RefCountedOwnershipPolicy() {}

  static T Clone(const T& t) {
    if (t) {
      t->AddRef();
    }
    return t;
  }

  static bool Release(const T& t) {
    if (t) {
      return t->Release();
    }
    return false;
  }

  void Swap(RefCountedOwnershipPolicy& other) {}
};

}  // namespace ptr
}  // namespace base

#endif  // BASE_PTR_REF_COUNTED_OWNERSHIP_POLICY_H_

