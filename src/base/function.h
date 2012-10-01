// Copyright (c) 2012 Cristian Patrasciuc. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef BASE_FUNCTION_H_
#define BASE_FUNCTION_H_

#include "base/callable.h"

namespace base {

template <typename Signature>
class Function : public Callable<Signature> {};

template <class R>
class Function<R(void)> : public Callable<R(void)> {
 public:
  Function(R (*f)(void)) : f_(f) {}

  virtual R operator()(void) const {
    return (*f_)();
  }

 private:
  R (*f_)(void);
};

template <class R, class A1>
class Function<R(A1)> : public Callable<R(A1)> {
 public:
  Function(R (*f)(A1)) : f_(f) {}

  virtual R operator()(A1 a1) const {
    return (*f_)(a1);
  }

 private:
  R (*f_)(A1);
};

template <class R, class A1, class A2>
class Function<R(A1, A2)> : public Callable<R(A1, A2)> {
 public:
  Function(R (*f)(A1, A2)) : f_(f) {}

  virtual R operator()(A1 a1, A2 a2) const {
    return (*f_)(a1, a2);
  }

 private:
  R (*f_)(A1, A2);
};

template <class R, class A1, class A2, class A3>
class Function<R(A1, A2, A3)> : public Callable<R(A1, A2, A3)> {
 public:
  Function(R (*f)(A1, A2, A3)) : f_(f) {}

  virtual R operator()(A1 a1, A2 a2, A3 a3) const {
    return (*f_)(a1, a2, a3);
  }

 private:
  R (*f_)(A1, A2, A3);
};

template <class R, class A1, class A2, class A3, class A4>
class Function<R(A1, A2, A3, A4)> : public Callable<R(A1, A2, A3, A4)> {
 public:
  Function(R (*f)(A1, A2, A3, A4)) : f_(f) {}

  virtual R operator()(A1 a1, A2 a2, A3 a3, A4 a4) const {
    return (*f_)(a1, a2, a3, a4);
  }

 private:
  R (*f_)(A1, A2, A3, A4);
};

}  // namespace base

#endif  // BASE_FUNCTION_H_

