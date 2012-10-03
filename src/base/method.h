// Copyright (c) 2012 Cristian Patrasciuc. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef BASE_METHOD_H_
#define BASE_METHOD_H_

#include "base/callable.h"

namespace base {

template <typename Signature>
class Method : public Callable<Signature> {};

template <class T, class R>
class Method<R(T::*)(void)> : public Callable<R(void)> {
 public:
  Method(R (T::*f)(void), T* t) : f_(f), t_(t) {}

  virtual R operator()(void) const {
    return (t_->*f_)();
  }

 private:
  R (T::*f_)(void);
  T* t_;
};

template <class T, class R, class A1>
class Method<R(T::*)(A1)> : public Callable<R(A1)> {
 public:
  Method(R (T::*f)(A1), T* t) : f_(f), t_(t) {}

  virtual R operator()(A1 a1) const {
    return (t_->*f_)(a1);
  }

 private:
  R (T::*f_)(A1);
  T* t_;
};

template <class T, class R, class A1, class A2>
class Method<R(T::*)(A1, A2)> : public Callable<R(A1, A2)> {
 public:
  Method(R (T::*f)(A1, A2), T* t) : f_(f), t_(t) {}

  virtual R operator()(A1 a1, A2 a2) const {
    return (t_->*f_)(a1, a2);
  }

 private:
  R (T::*f_)(A1, A2);
  T* t_;
};

template <class T, class R, class A1, class A2, class A3>
class Method<R(T::*)(A1, A2, A3)> : public Callable<R(A1, A2, A3)> {
 public:
  Method(R (T::*f)(A1, A2, A3), T* t) : f_(f), t_(t) {}

  virtual R operator()(A1 a1, A2 a2, A3 a3) const {
    return (t_->*f_)(a1, a2, a3);
  }

 private:
  R (T::*f_)(A1, A2, A3);
  T* t_;
};

template <class T, class R, class A1, class A2, class A3, class A4>
class Method<R(T::*)(A1, A2, A3, A4)> : public Callable<R(A1, A2, A3, A4)> {
 public:
  Method(R (T::*f)(A1, A2, A3, A4), T* t) : f_(f), t_(t) {}

  virtual R operator()(A1 a1, A2 a2, A3 a3, A4 a4) const {
    return (t_->*f_)(a1, a2, a3, a4);
  }

 private:
  R (T::*f_)(A1, A2, A3, A4);
  T* t_;
};

// Const Methods

template <class T, class R>
class Method<R(T::*)(void) const> : public Callable<R(void)> {
 public:
  Method(R (T::*f)(void) const, T* t) : f_(f), t_(t) {}

  virtual R operator()(void) const {
    return (t_->*f_)();
  }

 private:
  R (T::*f_)(void) const;
  T* t_;
};

template <class T, class R, class A1>
class Method<R(T::*)(A1) const> : public Callable<R(A1)> {
 public:
  Method(R (T::*f)(A1) const, T* t) : f_(f), t_(t) {}

  virtual R operator()(A1 a1) const {
    return (t_->*f_)(a1);
  }

 private:
  R (T::*f_)(A1) const;
  T* t_;
};

template <class T, class R, class A1, class A2>
class Method<R(T::*)(A1, A2) const> : public Callable<R(A1, A2)> {
 public:
  Method(R (T::*f)(A1, A2) const, T* t) : f_(f), t_(t) {}

  virtual R operator()(A1 a1, A1 a2) const {
    return (t_->*f_)(a1, a2);
  }

 private:
  R (T::*f_)(A1, A2) const;
  T* t_;
};

template <class T, class R, class A1, class A2, class A3>
class Method<R(T::*)(A1, A2, A3) const> : public Callable<R(A1, A2, A3)> {
 public:
  Method(R (T::*f)(A1, A2, A3) const, T* t) : f_(f), t_(t) {}

  virtual R operator()(A1 a1, A2 a2, A3 a3) const {
    return (t_->*f_)(a1, a2, a3);
  }

 private:
  R (T::*f_)(A1, A2, A3) const;
  T* t_;
};

template <class T, class R, class A1, class A2, class A3, class A4>
class Method<R(T::*)(A1, A2, A3, A4) const>
    : public Callable<R(A1, A2, A3, A4)> {
 public:
  Method(R (T::*f)(A1, A2, A3, A4) const, T* t) : f_(f), t_(t) {}

  virtual R operator()(A1 a1, A2 a2, A3 a3, A4 a4) const {
    return (t_->*f_)(a1, a2, a3, a4);
  }

 private:
  R (T::*f_)(A1, A2, A3, A4) const;
  T* t_;
};

}  // namespace base

#endif  // BASE_METHOD_H_

