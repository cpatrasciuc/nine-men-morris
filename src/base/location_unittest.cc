// Copyright (c) 2012 Cristian Patrasciuc. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include <string>

#include "base/location.h"
#include "gtest/gtest.h"

namespace base {
namespace {

Location GetTestLocation() {
  return FROM_HERE;
}

TEST(LocationTest, FromHere) {
  Location loc = GetTestLocation();
  EXPECT_EQ("GetTestLocation", loc.function());
  EXPECT_EQ("location_unittest.cc", loc.file_name());
  EXPECT_EQ(14, loc.line_number());
}

}  // anonymous namespace
}  // namespace base
