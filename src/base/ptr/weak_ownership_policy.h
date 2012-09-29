// Copyright (c) 2012 Cristian Patrasciuc. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef BASE_PTR_WEAK_OWNERSHIP_POLICY_H_
#define BASE_PTR_WEAK_OWNERSHIP_POLICY_H_

#include <algorithm>

#include "base/basic_macros.h"

namespace base {
namespace ptr {

template <class T>
class WeakOwnershipPolicy {
 public:
  class Observer {
   public:
    Observer() : is_valid_(false) {}
    explicit Observer(bool is_valid) : is_valid_(is_valid) {}
    ~Observer() {}

    bool is_valid() const {
      return is_valid_;
    }

    void set_valid(bool is_valid) const {
      is_valid_ = is_valid;
    }

   private:
    mutable bool is_valid_;

    DISALLOW_COPY_AND_ASSIGN(Observer);
  };

  explicit WeakOwnershipPolicy(const T& t = NULL)
      : observer_(new Observer(t != NULL)) {}
  ~WeakOwnershipPolicy() {
    // |observer_| should already be removed from the observers list
    delete observer_;
  }

  T Clone(const T& t) {
    if (t) {
      t->AddWeakRef(observer_);
      observer_->set_valid(true);
    }
    return t;
  }

  bool Release(const T& t) {
    if (observer_->is_valid() && t) {
      t->ReleaseWeakRef(observer_);
    }
    return false;
  }

  inline bool IsValid() const {
    return observer_->is_valid();
  }

  void Swap(WeakOwnershipPolicy& other) {
    std::swap(observer_, other.observer_);
  }

 private:
  Observer* observer_;
};

}  // namespace ptr
}  // namespace base

#endif  // BASE_PTR_WEAK_OWNERSHIP_POLICY_H_

