// Copyright (c) 2013 Cristian Patrasciuc. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef GRAPHICS_OUT_OF_PROCESS_TEST_H_
#define GRAPHICS_OUT_OF_PROCESS_TEST_H_

#include <string>

#include "gtest/gtest.h"

namespace graphics {

class OutOfProcessTest : public ::testing::Test {
 public:
  void RunTestInExternalProcess(const std::string& test_name);
};

#define OOP_TEST_F(test_class, test_name) \
  TEST_F(OutOfProcessTest, test_class ## _ ## test_name) { \
    RunTestInExternalProcess(#test_class "." #test_name); \
  }

}  // namespace graphics

#endif  // GRAPHICS_OUT_OF_PROCESS_TEST_H_

