// Copyright (c) 2013 Cristian Patrasciuc. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef BASE_SUPPORTS_LISTENER_H_
#define BASE_SUPPORTS_LISTENER_H_

#include <algorithm>
#include <deque>

#include "base/basic_macros.h"
#include "base/log.h"

namespace base {

// Template used to add support for a given type of listeners to an existing
// class C. If the class C has to support listeners of type T, C must extend
// SupportsListener<T>. The way in which events are fired it's class C's
// responsibility. This template takes care only of listener management.
template <class ListenerType>
class SupportsListener {
 public:
  typedef std::deque<ListenerType*> ListenerList;
  typedef typename ListenerList::const_iterator ListenerIter;

  ~SupportsListener() {}

  void AddListener(ListenerType* listener) {
    listeners_.push_back(listener);
  }

  void RemoveListener(ListenerType* listener) {
    typename ListenerList::iterator it =
        std::find(listeners_.begin(), listeners_.end(), listener);
    DCHECK(it != listeners_.end());
    listeners_.erase(it);
  }

 protected:
  SupportsListener() : listeners_() {}

  // This returns a copy of the listener list so that external users cannot
  // modify it directly. When a listener is notified, it can remove itself from
  // the listener list using RemoveListener(). If there would be external users
  // of this class iterating through the listener list when the removal occurs,
  // the application would crash.
  const ListenerList listeners() const { return listeners_; }

 private:
  ListenerList listeners_;

  DISALLOW_COPY_AND_ASSIGN(SupportsListener<ListenerType>);
};

}  // namespace base

#endif  // BASE_SUPPORTS_LISTENER_H_

