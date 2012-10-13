// Copyright (c) 2012 Cristian Patrasciuc. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "base/threading/location.h"

namespace base {
namespace threading {

Location::Location(const char* function, const char* file_name, int line_number)
    : function_(function), file_name_(file_name), line_number_(line_number) {}

}  // namespace threading
}  // namespace base
