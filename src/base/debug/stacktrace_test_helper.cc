// Copyright (c) 2012 Cristian Patrasciuc. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "base/debug/stacktrace_test_helper.h"

#include <ostream>
#include <string>
#include <vector>

#include "base/base_export.h"
#include "base/debug/stacktrace.h"

namespace base {
namespace debug {

const char kCStyleFunctionName[] = "CStyleFunction";

void CStyleFunction(std::ostream* out) {
  base::debug::PrintStackTrace(32, out);
}

std::vector<std::string> StackTraceTestHelper::GetExpectedStackTrace() {
  const std::string class_name = "base::debug::StackTraceTestHelper::";
  std::vector<std::string> result;
  result.push_back("qux");
  result.push_back("baz");
  result.push_back("bar");
  result.push_back("foo");
  for (unsigned int i = 0; i < result.size(); ++i) {
    result[i].insert(0, class_name);
  }
  return result;
}

void StackTraceTestHelper::foo(std::ostream* out, const int depth) const {
  bar(out, depth);
}

void StackTraceTestHelper::bar(std::ostream* out, const int depth) const {
  baz(out, depth);
}

void StackTraceTestHelper::baz(std::ostream* out, const int depth) const {
  qux(out, depth);
}

void StackTraceTestHelper::qux(std::ostream* out, const int depth) const {
  PrintStackTrace(depth, out);
}

}  // namespace debug
}  // namespace base
