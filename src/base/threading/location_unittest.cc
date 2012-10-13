// Copyright (c) 2012 Cristian Patrasciuc. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include <string.h>

#include <string>

#include "base/threading/location.h"
#include "gtest/gtest.h"

namespace base {
namespace threading {
namespace {

Location GetTestLocation() {
  return FROM_HERE;
}

TEST(LocationTest, FromHere) {
  Location loc = GetTestLocation();
  EXPECT_EQ("GetTestLocation", loc.function());
  EXPECT_EQ(std::string("location_unittest.cc"),
            basename(loc.file_name().c_str()));
  EXPECT_EQ(17, loc.line_number());
}

}  // anonymous namespace
}  // namespace threading
}  // namespace base
