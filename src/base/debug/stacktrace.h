// Copyright (c) 2012 Cristian Patrasciuc. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef BASE_DEBUG_STACKTRACE_H_
#define BASE_DEBUG_STACKTRACE_H_

#include <iostream>

#include "base/base_export.h"

namespace base {
namespace debug {

// Prints the stack trace on the current thread to the given output stream.
// Only the |max_depth| most recent function calls are printed.
void BASE_EXPORT PrintStackTrace(int max_depth = 64,
                                 std::ostream* out = &std::cerr);

// Dump the stack trace for each thread to STDERR
void BASE_EXPORT DumpStackTraceForAllThreads();

}  // namespace debug
}  // namespace base

#endif  // BASE_DEBUG_STACKTRACE_H_

