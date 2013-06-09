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
#include "base/string_util.h"
#include "gtest/gtest.h"

namespace base {
namespace debug {
namespace {

TEST(PrintStackTrace, NoArguments) {
  PrintStackTrace();
}

TEST(PrintStackTrace, DEBUG_ONLY_TEST(PrintToString)) {
  std::ostringstream out;
  StackTraceTestHelper helper;
  std::vector<std::string> expected_stack =
      StackTraceTestHelper::GetExpectedStackTrace();
  expected_stack.insert(expected_stack.begin(), "PrintStackTrace");
  helper.foo(&out, expected_stack.size() * 10);
  LOG(DEBUG) << out.str();
  std::vector<std::string> lines;
  SplitString(out.str(), &lines, "\r\n");
  ASSERT_GT(lines.size(), expected_stack.size());
  for (unsigned int i = 0; i < expected_stack.size(); ++i) {
    EXPECT_NE(std::string::npos, lines[i].find(expected_stack[i]));
  }
}

TEST(PrintStackTrace, DEBUG_ONLY_TEST(SmallDepth)) {
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
  std::vector<std::string> lines;
  SplitString(out.str(), &lines, "\r\n");
  lines.pop_back();  // The last line just says "... and more"
  ASSERT_EQ(depth, lines.size());
  for (unsigned int i = 0; i < lines.size(); ++i) {
    EXPECT_NE(std::string::npos, lines[i].find(expected_stack[i]));
  }
}

TEST(PrintStackTrace, DEBUG_ONLY_TEST(CStyleFunction)) {
  std::ostringstream out;
  CStyleFunction(&out);
  std::string stacktrace(out.str());
  LOG(DEBUG) << stacktrace;
  EXPECT_NE(std::string::npos, stacktrace.find(kCStyleFunctionName));
}

TEST(PrintStackTrace, DumpStackTraceForAllThreads) {
  DumpStackTraceForAllThreads();
}

// TODO(debug): This fails in static library builds.
TEST(PrintStackTraceDeathTest, DISABLED_StackTraceDumpOnCrash) {
  ::testing::FLAGS_gtest_death_test_style = "threadsafe";
  EnableStackTraceDumpOnCrash();
  int* p = NULL;
  ASSERT_DEATH(*p = 69, "StackTraceDumpOnCrash");
}

}  // anonymous namespace
}  // namespace debug
}  // namespace base
