// Copyright (c) 2012 Cristian Patrasciuc. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include <string>

#include "base/location.h"
#include "gtest/gtest.h"

namespace base {
namespace {

// Force the preprocessor to consider this the 20th line so we don't have to
// update the unit test every time we make changes to this file.
//
// WARNING:
//   This also generates incorrect line numbers in stack traces for this file.
#define TEST_LINE_NUMBER 20
#line TEST_LINE_NUMBER
Location GetTestLocation() { return FROM_HERE; }

TEST(LocationTest, FromHere) {
  Location loc = GetTestLocation();
  EXPECT_EQ("GetTestLocation", loc.function());
  EXPECT_EQ("location_unittest.cc", loc.file_name().value());
  EXPECT_EQ(TEST_LINE_NUMBER, loc.line_number());
}

}  // anonymous namespace
}  // namespace base
