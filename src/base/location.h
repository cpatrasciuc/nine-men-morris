// Copyright (c) 2012 Cristian Patrasciuc. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef BASE_LOCATION_H_
#define BASE_LOCATION_H_

#include <string>

#include "base/base_export.h"
#include "base/threading/thread.h"

namespace base {

class BASE_EXPORT Location {
 public:
  // TODO(threading): Add thread ID
  Location(const char* function,
           const char* file_name,
           int line_number,
           const base::threading::Thread* thread);

  const std::string& function() const {
    return function_;
  }

  const std::string& file_name() const {
    return file_name_;
  }

  int line_number() const {
    return line_number_;
  }

  // WARNING: This might return NULL if the location refers to the main thread.
  const base::threading::Thread* thread() const {
    return thread_;
  }

 private:
  const std::string function_;
  const std::string file_name_;
  const int line_number_;
  const base::threading::Thread* thread_;
};

#define FROM_HERE \
  base::Location(__FUNCTION__, \
                 __FILE__, \
                 __LINE__, \
                 base::threading::Thread::Current())

}  // namespace base

#endif  // BASE_LOCATION_H_

