// Copyright (c) 2012 Cristian Patrasciuc. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef BASE_DEBUG_STACKTRACE_H_
#define BASE_DEBUG_STACKTRACE_H_

#include <iosfwd>
#include <iostream>

namespace base {
namespace debug {

/* Prints the stacktrace on the current thread to the given output stream */
void PrintStackTrace(std::ostream* out = &std::cerr);

}  // namespace debug
}  // namespace base

#endif  // BASE_DEBUG_STACKTRACE_H_

