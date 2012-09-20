// Copyright (c) 2012 Cristian Patrasciuc. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include <ostream>
#include <sstream>
#include <string>
#include <vector>

#include "base/basic_macros.h"
#include "base/debug/stacktrace.h"
#include "base/debug/stacktrace_test_helper.h"
#include "base/log.h"
#include "gtest/gtest.h"

namespace base {
namespace debug {
namespace {

// TODO(strings): Add a string split utility method
std::vector<std::string> StringSplit(std::string s) {
  std::vector<std::string> lines;
  std::istringstream in(s);
  std::string item;
  while (std::getline(in, item)) {
    if (item.size() > 0) {
      lines.push_back(item);
    }
  }
  return lines;
}

TEST(PrintStackTrace, NoArguments) {
  PrintStackTrace();
}

TEST(PrintStackTrace, PrintToString) {
  std::ostringstream out;
  StackTraceTestHelper helper;
  std::vector<std::string> expected_stack =
      StackTraceTestHelper::GetExpectedStackTrace();
  expected_stack.insert(expected_stack.begin(), "PrintStackTrace");
  helper.foo(&out, expected_stack.size() * 10);
  LOG(DEBUG) << out.str();
  std::vector<std::string> lines = StringSplit(out.str());
  ASSERT_GT(lines.size(), expected_stack.size());
  for (unsigned int i = 0; i < expected_stack.size(); ++i) {
    EXPECT_NE(std::string::npos, lines[i].find(expected_stack[i]));
  }
}

TEST(PrintStackTrace, SmallDepth) {
  std::ostringstream out;
  StackTraceTestHelper helper;
  std::vector<std::string> expected_stack =
      StackTraceTestHelper::GetExpectedStackTrace();
  expected_stack.insert(expected_stack.begin(), "PrintStackTrace");
  const unsigned int frames_to_discard = 2;
  ASSERT_GT(expected_stack.size(), frames_to_discard);
  const unsigned int depth = expected_stack.size() - frames_to_discard;
  helper.foo(&out, depth);
  LOG(DEBUG) << out.str();
  std::vector<std::string> lines = StringSplit(out.str());
  lines.pop_back();  // The last line just says "... and more"
  ASSERT_EQ(depth, lines.size());
  for (unsigned int i = 0; i < lines.size(); ++i) {
    EXPECT_NE(std::string::npos, lines[i].find(expected_stack[i]));
  }
}

TEST(PrintStackTrace, CStyleFunction) {
  std::ostringstream out;
  CStyleFunction(&out);
  std::string stacktrace(out.str());
  LOG(DEBUG) << stacktrace;
  EXPECT_NE(std::string::npos, stacktrace.find(kCStyleFunctionName));
}

TEST(PrintStackTrace, DumpStackTraceForAllThreads) {
  DumpStackTraceForAllThreads();
}

// This test generates a Segmentation fault.
// Don't enable it unless you specifically debug the crash handler.
TEST(PrintStackTrace, DISABLED_StackTraceDumpOnCrash) {
  EnableStackTraceDumpOnCrash();
  int* p = NULL;
  *p = 69;
}

}  // anonymous namespace
}  // namespace debug
}  // namespace base
