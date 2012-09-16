// Copyright (c) 2012 Cristian Patrasciuc. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "base/log.h"
#include "gtest/gtest.h"

TEST(MyTest, BlaBla) {
  LOG(INFO) << "Some logging";
  LOG_IF(DEBUG, 5 < 10) << "Bla bla";
  LOG_IF(WARNING, 4 < 3) << "ION";
  std::cerr << "Manual output" << std::endl;
  LOG(INFO) << "This is some useful info";
  LOG_IF(ERROR, true) << "The pointer is NULL";
}
