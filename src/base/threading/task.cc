// Copyright (c) 2012 Cristian Patrasciuc. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "base/threading/task.h"

#include "base/log.h"

using base::ptr::scoped_ptr;

namespace base {
namespace threading {

Task::Task(Location location, Closure* closure, Closure* callback)
    : location_(location), closure_(closure), callback_(callback) {}

void Task::Run() {
  DCHECK(Get(closure_));
  (*closure_)();
  // TODO(threading) : Post the callback on the origin thread
  if (callback_) {
    (*callback_)();
  }
}

}  // namespace threading
}  // namespace base
