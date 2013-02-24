// Copyright (c) 2013 Cristian Patrasciuc. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include <fstream>

#include "base/file_util.h"
#include "gtest/gtest.h"

namespace base {
namespace {

TEST(ScopedTempDir, Basic) {
  FilePath temp_dir_path;
  FilePath temp_file_path;
  {
    ScopedTempDir temp_dir("");
    ASSERT_TRUE(temp_dir.Create());
    temp_dir_path = temp_dir.Get();
    ASSERT_TRUE(temp_dir_path.Exists());

    temp_file_path = temp_dir_path.Append(FILE_PATH_LITERAL("file.txt"));
    std::ofstream out(temp_file_path.value().c_str());
    out << "Test contents" << std::endl;
    out.close();
    EXPECT_TRUE(temp_file_path.Exists());
  }
  EXPECT_FALSE(temp_dir_path.Exists());
  EXPECT_FALSE(temp_file_path.Exists());
}

}  // anonymous namespace
}  // namespace base
