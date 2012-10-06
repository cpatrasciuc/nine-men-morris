// Copyright (c) 2012 Cristian Patrasciuc. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef BASE_CALLABLE_H_
#define BASE_CALLABLE_H_

namespace base {

template <typename Signature>
class Callable;

template <typename R>
class Callable<R(void)> {
 public:
  virtual ~Callable() {}
  virtual R operator()(void) const = 0;
};

template <typename R, typename A1>
class Callable<R(A1)> {
 public:
  virtual ~Callable() {}
  virtual R operator()(A1) const = 0;
};

template <typename R, typename A1, typename A2>
class Callable<R(A1, A2)> {
 public:
  virtual ~Callable() {}
  virtual R operator()(A1, A2) const = 0;
};

template <typename R, typename A1, typename A2, typename A3>
class Callable<R(A1, A2, A3)> {
 public:
  virtual ~Callable() {}
  virtual R operator()(A1, A2, A3) const = 0;
};

template <typename R, typename A1, typename A2, typename A3, typename A4>
class Callable<R(A1, A2, A3, A4)> {
 public:
  virtual ~Callable() {}
  virtual R operator()(A1, A2, A3, A4) const = 0;
};

template <class R, class A1, class A2, class A3, class A4, class A5>
class Callable<R(A1, A2, A3, A4, A5)> {
 public:
  virtual ~Callable() {}
  virtual R operator()(A1, A2, A3, A4, A5) const = 0;
};

typedef Callable<void(void)> Closure;

}  // namespace base

#endif  // BASE_CALLABLE_H_

