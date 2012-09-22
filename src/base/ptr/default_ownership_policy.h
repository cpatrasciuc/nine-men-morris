// Copyright (c) 2012 Cristian Patrasciuc. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef BASE_PTR_DEFAULT_OWNERSHIP_POLICY_H_
#define BASE_PTR_DEFAULT_OWNERSHIP_POLICY_H_

#include "base/basic_macros.h"

namespace base {
namespace ptr {

template <class T>
class DefaultOwnershipPolicy {
 public:
  DefaultOwnershipPolicy() {}
  explicit DefaultOwnershipPolicy(const T& t) {}
  ~DefaultOwnershipPolicy() {}

  T Clone(const T& t) {
    return t;
  }

  bool Release(const T& t) {
    return false;
  }

  void Swap(DefaultOwnershipPolicy& other) {}
};

}  // namespace ptr
}  // namespace base

#endif  // BASE_PTR_DEFAULT_OWNERSHIP_POLICY_H_

