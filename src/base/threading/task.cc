// Copyright (c) 2012 Cristian Patrasciuc. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "base/threading/task.h"

#include "base/log.h"
#include "base/threading/thread.h"

using base::ptr::scoped_ptr;

namespace base {
namespace threading {

Task::Task(Location location, Closure* closure, Closure* callback)
    : location_(location), closure_(closure), callback_(callback) {}

void Task::Run() {
  DCHECK(closure_);
  (*closure_)();
  // TODO(threading) : Post the callback on the origin thread
  if (callback_) {
    DCHECK(location_.thread());
    location_.thread()->SubmitTask(FROM_HERE, Get(callback_));
    Reset(callback_);
  }
}

}  // namespace threading
}  // namespace base
