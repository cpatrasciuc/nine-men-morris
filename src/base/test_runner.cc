// Copyright (c) 2012 Cristian Patrasciuc. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// TODO(cpatrasciuc): Auto-generated this file when a test target is built.

#include "base/debug/stacktrace.h"
#include "gtest/gtest.h"

int main(int argc, char **argv) {
  ::base::debug::EnableStackTraceDumpOnCrash();
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
