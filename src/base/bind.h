// Copyright (c) 2012 Cristian Patrasciuc. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef BASE_BIND_H_
#define BASE_BIND_H_

#include "base/callable.h"

namespace base {

template <class R, class A1>
class Binder10 : public Callable<R(void)> {
 public:
  Binder10(Callable<R(A1)>& c, A1 a1) : c_(c), a1_(a1) {}

  virtual R operator()() const {
    return c_(a1_);
  }

 private:
  Callable<R(A1)>& c_;
  A1 a1_;
};

template <class R, class A1>
Callable<R(void)>* Bind(Callable<R(A1)>& c, A1 a1) {
  return new Binder10<R, A1>(c, a1);
}

}  // namespace base

#endif  // BASE_BIND_H_

