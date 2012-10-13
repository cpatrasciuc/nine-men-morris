// Copyright (c) 2012 Cristian Patrasciuc. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef BASE_BINDERS_H_
#define BASE_BINDERS_H_

#include "base/bind_policy.h"
#include "base/callable.h"
#include "base/ptr/scoped_ptr.h"

namespace base {

template <class R, class A1, class P1>
class Binder10 : public Callable<R(void)> {
 public:
  Binder10(Callable<R(A1)>* c, P1 a1) : c_(c), a1_(a1) {}

  virtual R operator()(void) const {
    return (*c_)(BindPolicy<P1>::Forward(a1_));
  }

 private:
  base::ptr::scoped_ptr<Callable<R(A1)> > c_;
  P1 a1_;
};

template <class R, class A1, class A2, class P1>
class Binder21 : public Callable<R(A2)> {
 public:
  Binder21(Callable<R(A1, A2)>* c, P1 a1) : c_(c), a1_(a1) {}

  virtual R operator()(A2 a2) const {
    return (*c_)(BindPolicy<P1>::Forward(a1_), a2);
  }

 private:
  base::ptr::scoped_ptr<Callable<R(A1, A2)> > c_;
  P1 a1_;
};

template <class R, class A1, class A2, class P1, class P2>
class Binder20 : public Callable<R(void)> {
 public:
  Binder20(Callable<R(A1, A2)>* c, P1 a1, P2 a2) : c_(c), a1_(a1), a2_(a2) {}

  virtual R operator()() const {
    return (*c_)(BindPolicy<P1>::Forward(a1_), BindPolicy<P2>::Forward(a2_));
  }

 private:
  base::ptr::scoped_ptr<Callable<R(A1, A2)> > c_;
  P1 a1_;
  P2 a2_;
};

template <class R, class A1, class A2, class A3, class P1>
class Binder32 : public Callable<R(A2, A3)> {
 public:
  Binder32(Callable<R(A1, A2, A3)>* c, P1 a1) : c_(c), a1_(a1) {}

  virtual R operator()(A2 a2, A3 a3) const {
    return (*c_)(BindPolicy<P1>::Forward(a1_), a2, a3);
  }

 private:
  base::ptr::scoped_ptr<Callable<R(A1, A2, A3)> > c_;
  P1 a1_;
};

template <class R, class A1, class A2, class A3, class P1, class P2>
class Binder31 : public Callable<R(A3)> {
 public:
  Binder31(Callable<R(A1, A2, A3)>* c, P1 a1, P2 a2)
      : c_(c), a1_(a1), a2_(a2) {}

  virtual R operator()(A3 a3) const {
    return (*c_)(BindPolicy<P1>::Forward(a1_),
                 BindPolicy<P2>::Forward(a2_),
                 a3);
  }

 private:
  base::ptr::scoped_ptr<Callable<R(A1, A2, A3)> > c_;
  P1 a1_;
  P2 a2_;
};

template <class R, class A1, class A2, class A3, class P1, class P2, class P3>
class Binder30 : public Callable<R(void)> {
 public:
  Binder30(Callable<R(A1, A2, A3)>* c, P1 a1, P2 a2, P3 a3)
      : c_(c), a1_(a1), a2_(a2), a3_(a3) {}

  virtual R operator()() const {
    return (*c_)(BindPolicy<P1>::Forward(a1_),
                 BindPolicy<P2>::Forward(a2_),
                 BindPolicy<P3>::Forward(a3_));
  }

 private:
  base::ptr::scoped_ptr<Callable<R(A1, A2, A3)> > c_;
  P1 a1_;
  P2 a2_;
  P3 a3_;
};

template <class R, class A1, class A2, class A3, class A4, class P1>
class Binder43 : public Callable<R(A2, A3, A4)> {
 public:
  Binder43(Callable<R(A1, A2, A3, A4)>* c, P1 a1)
      : c_(c), a1_(a1) {}

  virtual R operator()(A2 a2, A3 a3, A4 a4) const {
    return (*c_)(BindPolicy<P1>::Forward(a1_), a2, a3, a4);
  }

 private:
  base::ptr::scoped_ptr<Callable<R(A1, A2, A3, A4)> > c_;
  P1 a1_;
};

template <class R, class A1, class A2, class A3, class A4, class P1, class P2>
class Binder42 : public Callable<R(A3, A4)> {
 public:
  Binder42(Callable<R(A1, A2, A3, A4)>* c, P1 a1, P2 a2)
      : c_(c), a1_(a1), a2_(a2) {}

  virtual R operator()(A3 a3, A4 a4) const {
    return (*c_)(BindPolicy<P1>::Forward(a1_),
                 BindPolicy<P2>::Forward(a2_),
                 a3,
                 a4);
  }

 private:
  base::ptr::scoped_ptr<Callable<R(A1, A2, A3, A4)> > c_;
  P1 a1_;
  P2 a2_;
};

template <class R, class A1, class A2, class A3, class A4,
          class P1, class P2, class P3>  // NOLINT(build/class)
class Binder41 : public Callable<R(A4)> {
 public:
  Binder41(Callable<R(A1, A2, A3, A4)>* c, P1 a1, P2 a2, P3 a3)
      : c_(c), a1_(a1), a2_(a2), a3_(a3) {}

  virtual R operator()(A4 a4) const {
    return (*c_)(BindPolicy<P1>::Forward(a1_),
                 BindPolicy<P2>::Forward(a2_),
                 BindPolicy<P3>::Forward(a3_),
                 a4);
  }

 private:
  base::ptr::scoped_ptr<Callable<R(A1, A2, A3, A4)> > c_;
  P1 a1_;
  P2 a2_;
  P3 a3_;
};

template <class R, class A1, class A2, class A3, class A4,
          class P1, class P2, class P3, class P4>  // NOLINT(build/class)
class Binder40 : public Callable<R(void)> {
 public:
  Binder40(Callable<R(A1, A2, A3, A4)>* c, P1 a1, P2 a2, P3 a3, P4 a4)
      : c_(c), a1_(a1), a2_(a2), a3_(a3), a4_(a4) {}

  virtual R operator()() const {
    return (*c_)(BindPolicy<P1>::Forward(a1_),
                 BindPolicy<P2>::Forward(a2_),
                 BindPolicy<P3>::Forward(a3_),
                 BindPolicy<P4>::Forward(a4_));
  }

 private:
  base::ptr::scoped_ptr<Callable<R(A1, A2, A3, A4)> > c_;
  P1 a1_;
  P2 a2_;
  P3 a3_;
  P4 a4_;
};

}  // namespace base

#endif  // BASE_BINDERS_H_

