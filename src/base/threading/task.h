// Copyright (c) 2012 Cristian Patrasciuc. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef BASE_THREADING_TASK_H_
#define BASE_THREADING_TASK_H_

#include "base/base_export.h"
#include "base/basic_macros.h"
#include "base/callable.h"
#include "base/ptr/scoped_ptr.h"

namespace base {
namespace threading {

// Represents a single method call together with the location from where it was
// created and (if needed) a callback. The Task object owns both the closure and
// the callback.
// The Task class is used to model an execution step inside a thread/task queue.
class BASE_EXPORT Task {
 public:
  Task(Location location, Closure* closure, Closure* callback = NULL);

  // Calls |closure_| on the current thread.If |callback_| is not NULL, it posts
  // a call to it on the origin thread that created this task object. The origin
  // thread is obtained from |location|.
  void Run();

 private:
  Location location_;
  base::ptr::scoped_ptr<Closure> closure_;
  base::ptr::scoped_ptr<Closure> callback_;

  DISALLOW_COPY_AND_ASSIGN(Task);
};

}  // namespace threading
}  // namespace base

#endif  // BASE_THREADING_TASK_H_

