// Copyright (c) 2012 Cristian Patrasciuc. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "base/debug/stacktrace.h"
#include "gtest/gtest.h"

namespace base {
namespace debug {
namespace {

TEST(StackTraceTest, PrintStackTrace) {
  PrintStackTrace();
}

TEST(StackTraceTest, DumpStackTraceForAllThreads) {
  DumpStackTraceForAllThreads();
}

}  // anonymous namespace
}  // namespace debug
}  // namespace base
