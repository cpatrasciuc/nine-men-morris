// Copyright (c) 2013 Cristian Patrasciuc. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include <unistd.h>

#include <cstdio>
#include <string>

#include "base/command_line.h"
#include "graphics/out_of_process_test.h"
#include "gtest/gtest.h"

namespace graphics {

void OutOfProcessTest::RunTestInExternalProcess(const std::string& test_name) {
  const int kMaxBufferSize = 256;
  const pid_t child_pid = fork();
  EXPECT_GE(child_pid, 0);
  if (child_pid == 0) {
    char gtest_arg[kMaxBufferSize];
    const int result = snprintf(gtest_arg, kMaxBufferSize, "--gtest_filter=%s",
        test_name.c_str());
    ASSERT_LE(result, kMaxBufferSize)
        << "Buffer too small. Consider a size of at least " << result;
    const std::string exe_name =
        base::CommandLine::ForCurrentProcess()->program_name();
    const char* argv[] = { exe_name.c_str(), gtest_arg, NULL };
    execvp(exe_name.c_str(), const_cast<char**>(argv));
    FAIL() << "execve() did not run successfully";
    _exit(1);
  } else {
    int status;
    const int result = waitpid(child_pid, &status, 0);
    EXPECT_GE(result, 0);
    EXPECT_TRUE(WIFEXITED(status));
    EXPECT_EQ(0, WEXITSTATUS(status));
  }
}

}  // namespace graphics
