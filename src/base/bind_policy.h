// Copyright (c) 2012 Cristian Patrasciuc. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef BASE_BIND_POLICY_H_
#define BASE_BIND_POLICY_H_

#include <memory>

#include "base/ptr/ref_ptr.h"
#include "base/ptr/weak_ptr.h"

namespace base {

template <typename T>
class BindPolicy {
 public:
  static T Forward(const T& t) { return t; }
};

template <typename T>
class BindPolicy<std::auto_ptr<T> > {
 public:
  static T* Forward(const std::auto_ptr<T>& t) { return t.get(); }
};

template <typename T>
class BindPolicy<base::ptr::ref_ptr<T> > {
 public:
  static T* Forward(const base::ptr::ref_ptr<T>& t) { return Get(t); }
};

template <typename T>
class BindPolicy<base::ptr::weak_ptr<T> > {
 public:
  static T* Forward(const base::ptr::weak_ptr<T>& t) { return Get(t); }
};

// Forward declaration
template <typename T> class ConstRefHolder;

template <typename T>
class BindPolicy<ConstRefHolder<T> > {
 public:
  static const T& Forward(const ConstRefHolder<T>& t) { return t.Get(); }
};

}  // namespace base

#endif  // BASE_BIND_POLICY_H_

