// Copyright (c) 2012 Cristian Patrasciuc. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef BASE_BIND_H_
#define BASE_BIND_H_

#include <memory>

#include "base/binders.h"
#include "base/callable.h"
#include "base/ptr/weak_ptr.h"

namespace base {

template <class R, class A1, class P1>
Callable<R(void)>* Bind(Callable<R(A1)>* c, P1 a1) {
  return new Binder10<R, A1, P1>(c, a1);
}

template <class R, class A1, class A2, class P1>
Callable<R(A2)>* Bind(Callable<R(A1, A2)>* c, P1 a1) {
  return new Binder21<R, A1, A2, P1>(c, a1);
}

template <class R, class A1, class A2, class P1, class P2>
Callable<R(void)>* Bind(Callable<R(A1, A2)>* c, P1 a1, P2 a2) {
  return new Binder20<R, A1, A2, P1, P2>(c, a1, a2);
}

template <class R, class A1, class A2, class A3, class P1>
Callable<R(A2, A3)>* Bind(Callable<R(A1, A2, A3)>* c, P1 a1) {
  return new Binder32<R, A1, A2, A3, P1>(c, a1);
}

template <class R, class A1, class A2, class A3, class P1, class P2>
Callable<R(A3)>* Bind(Callable<R(A1, A2, A3)>* c, P1 a1, P2 a2) {
  return new Binder31<R, A1, A2, A3, P1, P2>(c, a1, a2);
}

template <class R, class A1, class A2, class A3, class P1, class P2, class P3>
Callable<R(void)>* Bind(Callable<R(A1, A2, A3)>* c, P1 a1, P2 a2, P3 a3) {
  return new Binder30<R, A1, A2, A3, P1, P2, P3>(c, a1, a2, a3);
}

template <class R, class A1, class A2, class A3, class A4, class P1>
Callable<R(A2, A3, A4)>* Bind(Callable<R(A1, A2, A3, A4)>* c, P1 a1) {
  return new Binder43<R, A1, A2, A3, A4, P1>(c, a1);
}

template <class R, class A1, class A2, class A3, class A4, class P1, class P2>
Callable<R(A3, A4)>* Bind(Callable<R(A1, A2, A3, A4)>* c, P1 a1, P2 a2) {
  return new Binder42<R, A1, A2, A3, A4, P1, P2>(c, a1, a2);
}

template <class R, class A1, class A2, class A3, class A4,
          class P1, class P2, class P3>
Callable<R(A4)>* Bind(Callable<R(A1, A2, A3, A4)>* c, P1 a1, P2 a2, P3 a3) {
  return new Binder41<R, A1, A2, A3, A4, P1, P2, P3>(c, a1, a2, a3);
}

template <class R, class A1, class A2, class A3, class A4,
          class P1, class P2, class P3, class P4>
Callable<R(void)>* Bind(Callable<R(A1, A2, A3, A4)>* c,
    P1 a1, P2 a2, P3 a3, P4 a4) {
  return new Binder40<R, A1, A2, A3, A4, P1, P2, P3, P4>(c, a1, a2, a3, a4);
}

template <typename T>
inline std::auto_ptr<T> Owned(T* t) {
  return std::auto_ptr<T>(t);
}

template <typename T>
inline base::ptr::weak_ptr<T> Weak(T* t) {
  return base::ptr::weak_ptr<T>(t);
}

template <typename T>
class ConstRefHolder {
 public:
  explicit ConstRefHolder(const T* t) : t_(t) {}
  const T& Get() const { return *t_; }
 private:
  const T* t_;
};

template <typename T>
inline ConstRefHolder<T> ConstRef(T* t) {
  return ConstRefHolder<T>(t);
}

}  // namespace base

#endif  // BASE_BIND_H_

