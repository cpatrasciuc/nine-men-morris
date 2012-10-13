// Copyright (c) 2012 Cristian Patrasciuc. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef BASE_THREADING_LOCATION_H_
#define BASE_THREADING_LOCATION_H_

#include <string>

namespace base {
namespace threading {

class Location {
 public:
  // TODO(threading): Add thread ID
  Location(const char* function, const char* file_name, int line_number);

  const std::string& function() const {
    return function_;
  }

  const std::string& file_name() const {
    return file_name_;
  }

  int line_number() const {
    return line_number_;
  }

 private:
  std::string function_;
  std::string file_name_;
  int line_number_;
};

#define FROM_HERE base::threading::Location(__FUNCTION__, __FILE__, __LINE__)

}  // namespace threading
}  // namespace base

#endif  // BASE_THREADING_LOCATION_H_

