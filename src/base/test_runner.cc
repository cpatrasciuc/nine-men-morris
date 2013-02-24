// Copyright (c) 2012 Cristian Patrasciuc. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "base/debug/stacktrace.h"
#include "gtest/gtest.h"

int main(int argc, char **argv) {
  ::testing::FLAGS_gtest_death_test_style = "fast";
  ::base::debug::EnableStackTraceDumpOnCrash();
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
