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
    // TODO(threading): Write a death test for this
    DCHECK(location_.thread())
        << "Don't post tasks with callbacks from the main thread";

    // TODO(smart_pointer): Add a Release() method for scoped_ptr.
    Closure* callback = Get(callback_);
    GetImplAsRef(callback_) = NULL;
    location_.thread()->SubmitTask(FROM_HERE, callback);
  }
}

}  // namespace threading
}  // namespace base
