// Copyright (c) 2012 Cristian Patrasciuc. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "base/location.h"

#include <string.h>

#include "base/threading/thread.h"

namespace base {

Location::Location(const char* function,
                   const char* file_name,
                   int line_number,
                   base::threading::Thread* thread)
    : function_(function),
      // TODO(file_util): Add a FilePath class or file_util module
      file_name_(basename(file_name)),
      line_number_(line_number),
      thread_(thread) {
}

base::threading::Thread* GetCurrentThread() {
  return base::threading::Thread::Current();
}

}  // namespace base
