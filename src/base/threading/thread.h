// Copyright (c) 2012 Cristian Patrasciuc. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef BASE_THREADING_THREAD_H_
#define BASE_THREADING_THREAD_H_

#include <pthread.h>

#include <string>

#include "base/basic_macros.h"

namespace base {
namespace threading {

typedef pthread_t ThreadID;

class Thread {
 public:
  explicit Thread(std::string name);
  ~Thread();

  const std::string& name() const {
    return name_;
  }

  bool is_running() const {
    return is_running_;
  }

  // Starts |this| thread. Return |true| if the new thread was started
  // successfully; |false| otherwise.
  bool Start();

  // Blocks until |this| thread is over.
  void Join();

  // Check if the method is called on the thread represented by |t|.
  static bool CurrentlyOn(Thread* t);

 private:
  // Static method passed to pthread_create when starting a new thread. The
  // argument is a pointer to the current thread object. It just calls the
  // RunLoop() method on it.
  static void* StartThreadThunk(void* thread);

  void RunInternal();

  std::string name_;
  ThreadID thread_id;
  bool is_running_;

  DISALLOW_COPY_AND_ASSIGN(Thread);
};

}  // namespace threading
}  // namespace base

#endif  // BASE_THREADING_THREAD_H_

