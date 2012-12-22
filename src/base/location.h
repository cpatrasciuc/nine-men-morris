// Copyright (c) 2012 Cristian Patrasciuc. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef BASE_LOCATION_H_
#define BASE_LOCATION_H_

#include <string>

#include "base/base_export.h"
#include "base/file_path.h"

namespace base {

namespace threading {
class Thread;
}

class BASE_EXPORT Location {
 public:
  Location(const std::string& function,
           const FilePath& file_name,
           int line_number,
           base::threading::Thread* thread);

  const std::string& function() const {
    return function_;
  }

  const FilePath& file_name() const {
    return file_name_;
  }

  int line_number() const {
    return line_number_;
  }

  // WARNING: This might return NULL if the location refers to the main thread.
  base::threading::Thread* thread() const {
    return thread_;
  }

 private:
  const std::string function_;
  const FilePath file_name_;
  const int line_number_;
  base::threading::Thread* const thread_;
};

BASE_EXPORT base::threading::Thread* GetCurrentThread();

#define FROM_HERE \
  base::Location(__FUNCTION__, \
                 base::FilePath(__FILE__).BaseName(), \
                 __LINE__, \
                 base::GetCurrentThread())

}  // namespace base

#endif  // BASE_LOCATION_H_

