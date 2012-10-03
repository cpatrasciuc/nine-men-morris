// Copyright (c) 2012 Cristian Patrasciuc. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef BASE_BIND_H_
#define BASE_BIND_H_

#include "base/binders.h"
#include "base/callable.h"

namespace base {

template <class R, class A1>
Callable<R(void)>* Bind(Callable<R(A1)>* c, A1 a1) {
  return new Binder10<R, A1>(c, a1);
}

template <class R, class A1, class A2>
Callable<R(A2)>* Bind(Callable<R(A1, A2)>* c, A1 a1) {
  return new Binder21<R, A1, A2>(c, a1);
}

template <class R, class A1, class A2>
Callable<R(void)>* Bind(Callable<R(A1, A2)>* c, A1 a1, A2 a2) {
  return new Binder20<R, A1, A2>(c, a1, a2);
}

template <class R, class A1, class A2, class A3>
Callable<R(A2, A3)>* Bind(Callable<R(A1, A2, A3)>* c, A1 a1) {
  return new Binder32<R, A1, A2, A3>(c, a1);
}

template <class R, class A1, class A2, class A3>
Callable<R(A3)>* Bind(Callable<R(A1, A2, A3)>* c, A1 a1, A2 a2) {
  return new Binder31<R, A1, A2, A3>(c, a1, a2);
}

template <class R, class A1, class A2, class A3>
Callable<R(void)>* Bind(Callable<R(A1, A2, A3)>* c, A1 a1, A2 a2, A3 a3) {
  return new Binder30<R, A1, A2, A3>(c, a1, a2, a3);
}

}  // namespace base

#endif  // BASE_BIND_H_

