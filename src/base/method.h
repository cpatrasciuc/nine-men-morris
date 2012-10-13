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
class Method<R(T::*)(void)> : public Callable<R(T*)> {
 public:
  Method(R (T::*f)(void)) : f_(f) {}

  virtual R operator()(T* t) const {
    if (t) {
      return (t->*f_)();
    }
    return R();
  }

 private:
  R (T::*f_)(void);
};


template <class T, class R, class A1>
class Method<R(T::*)(A1)> : public Callable<R(T*, A1)> {
 public:
  Method(R (T::*f)(A1)) : f_(f) {}

  virtual R operator()(T* t, A1 a1) const {
    if (t) {
      return (t->*f_)(a1);
    }
    return R();
  }

 private:
  R (T::*f_)(A1);
};

template <class T, class R, class A1, class A2>
class Method<R(T::*)(A1, A2)> : public Callable<R(T*, A1, A2)> {
 public:
  Method(R (T::*f)(A1, A2)) : f_(f) {}

  virtual R operator()(T* t, A1 a1, A2 a2) const {
    if (t) {
      return (t->*f_)(a1, a2);
    }
    return R();
  }

 private:
  R (T::*f_)(A1, A2);
};

template <class T, class R, class A1, class A2, class A3>
class Method<R(T::*)(A1, A2, A3)> : public Callable<R(T*, A1, A2, A3)> {
 public:
  Method(R (T::*f)(A1, A2, A3)) : f_(f) {}

  virtual R operator()(T* t, A1 a1, A2 a2, A3 a3) const {
    if (t) {
      return (t->*f_)(a1, a2, a3);
    }
    return R();
  }

 private:
  R (T::*f_)(A1, A2, A3);
};

template <class T, class R, class A1, class A2, class A3, class A4>
class Method<R(T::*)(A1, A2, A3, A4)> : public Callable<R(T*, A1, A2, A3, A4)> {
 public:
  Method(R (T::*f)(A1, A2, A3, A4)) : f_(f) {}

  virtual R operator()(T* t, A1 a1, A2 a2, A3 a3, A4 a4) const {
    if (t) {
      return (t->*f_)(a1, a2, a3, a4);
    }
    return R();
  }

 private:
  R (T::*f_)(A1, A2, A3, A4);
};

// Const Methods

template <class T, class R>
class Method<R(T::*)(void) const> : public Callable<R(const T*)> {
 public:
  Method(R (T::*f)(void) const) : f_(f) {}

  virtual R operator()(const T* t) const {
    if (t) {
      return (t->*f_)();
    }
    return R();
  }

 private:
  R (T::*f_)(void) const;
};

template <class T, class R, class A1>
class Method<R(T::*)(A1) const> : public Callable<R(const T*, A1)> {
 public:
  Method(R (T::*f)(A1) const) : f_(f) {}

  virtual R operator()(const T* t, A1 a1) const {
    if (t) {
      return (t->*f_)(a1);
    }
    return R();
  }

 private:
  R (T::*f_)(A1) const;
};

template <class T, class R, class A1, class A2>
class Method<R(T::*)(A1, A2) const> : public Callable<R(const T*, A1, A2)> {
 public:
  Method(R (T::*f)(A1, A2) const) : f_(f) {}

  virtual R operator()(const T* t, A1 a1, A1 a2) const {
    if (t) {
      return (t->*f_)(a1, a2);
    }
    return R();
  }

 private:
  R (T::*f_)(A1, A2) const;
};

template <class T, class R, class A1, class A2, class A3>
class Method<R(T::*)(A1, A2, A3) const>
    : public Callable<R(const T*, A1, A2, A3)> {
 public:
  Method(R (T::*f)(A1, A2, A3) const) : f_(f) {}

  virtual R operator()(const T* t, A1 a1, A2 a2, A3 a3) const {
    if (t) {
      return (t->*f_)(a1, a2, a3);
    }
    return R();
  }

 private:
  R (T::*f_)(A1, A2, A3) const;
};

template <class T, class R, class A1, class A2, class A3, class A4>
class Method<R(T::*)(A1, A2, A3, A4) const>
    : public Callable<R(const T*, A1, A2, A3, A4)> {
 public:
  Method(R (T::*f)(A1, A2, A3, A4) const) : f_(f) {}

  virtual R operator()(const T* t, A1 a1, A2 a2, A3 a3, A4 a4) const {
    if (t) {
      return (t->*f_)(a1, a2, a3, a4);
    }
    return R();
  }

 private:
  R (T::*f_)(A1, A2, A3, A4) const;
};

}  // namespace base

#endif  // BASE_METHOD_H_

