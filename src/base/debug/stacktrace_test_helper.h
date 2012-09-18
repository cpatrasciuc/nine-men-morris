// Copyright (c) 2012 Cristian Patrasciuc. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef BASE_DEBUG_STACKTRACE_TEST_HELPER_H_
#define BASE_DEBUG_STACKTRACE_TEST_HELPER_H_

#include <ostream>
#include <string>
#include <vector>

#include "base/base_export.h"

namespace base {
namespace debug {

BASE_EXPORT extern const char kCStyleFunctionName[];

extern "C" {
  BASE_EXPORT void CStyleFunction(std::ostream* out);
}

class BASE_EXPORT StackTraceTestHelper {
 public:
  void foo(std::ostream* out, const int depth) const;
  void bar(std::ostream* out, const int depth) const;
  void baz(std::ostream* out, const int depth) const;
  void qux(std::ostream* out, const int depth) const;

  // Returns the name of the methods in the order in which they should
  // appear in a stack trace, starting with the most recent one.
  static std::vector<std::string> GetExpectedStackTrace();
};

}  // namespace debug
}  // namespace base

#endif  // BASE_DEBUG_STACKTRACE_TEST_HELPER_H_

