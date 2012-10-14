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
  if (callback_) {
    DCHECK(location_.thread())
        << "Don't post tasks with callbacks from the main thread";
    location_.thread()->SubmitTask(FROM_HERE, Release(&callback_));
  }
}

}  // namespace threading
}  // namespace base
