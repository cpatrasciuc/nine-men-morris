// Copyright (c) 2013 Cristian Patrasciuc. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "base/command_line.h"
#include "base/debug/stacktrace.h"
#include "gtest/gtest.h"

int main(int argc, char **argv) {
  ::testing::FLAGS_gtest_death_test_style = "threadsafe";
  ::base::debug::EnableStackTraceDumpOnCrash();
  ::base::CommandLine::ForCurrentProcess()->Init(argc, argv);
  if (!::base::CommandLine::ForCurrentProcess()->HasSwitch("--gtest_filter")) {
    ::testing::FLAGS_gtest_filter = "OutOfProcessTest.*";
  }
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}

