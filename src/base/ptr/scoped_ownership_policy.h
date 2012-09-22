// Copyright (c) 2012 Cristian Patrasciuc. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef BASE_PTR_SCOPED_OWNERSHIP_POLICY_H_
#define BASE_PTR_SCOPED_OWNERSHIP_POLICY_H_

#include "base/ptr/default_ownership_policy.h"

namespace base {
namespace ptr {

template <class T>
class ScopedOwnershipPolicy : public DefaultOwnershipPolicy<T> {
 public:
  ScopedOwnershipPolicy() {}
  explicit ScopedOwnershipPolicy(const T& t) {}
  ~ScopedOwnershipPolicy() {}

  // When the smart pointer gets out of scope we always destroy the object
  bool Release(const T& t) {
    return true;
  }

  void Swap(ScopedOwnershipPolicy& other) {}
};

}  // namespace ptr
}  // namespace base

#endif  // BASE_PTR_SCOPED_OWNERSHIP_POLICY_H_

