// Copyright (c) 2012 Cristian Patrasciuc. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include <glob.h>
#include <unistd.h>

#include <cstdio>
#include <iostream>
#include <map>
#include <string>

#include "base/basic_macros.h"
#include "base/console.h"
#include "base/log.h"
#include "gtest/gtest.h"

namespace base {
namespace {

TEST(ConsoleDeathTest, IsColoredOutputSupported) {
  ::testing::FLAGS_gtest_death_test_style = "threadsafe";
  const std::string passed_message("PASSED");
  std::map<std::string, bool> files;

  // /dev/null should not support colors
  files.insert(std::make_pair("/dev/null", false));

  // Search for a tty device
  glob_t paths;
  if (!glob("/dev/*tty*", 0, NULL, &paths)) {
    if (paths.gl_pathc > 0) {
      files.insert(std::make_pair(paths.gl_pathv[0], true));
      LOG(INFO) << "Using TTY device: " << paths.gl_pathv[0];
    } else {
      LOG(INFO) << "Could not find any TTY devices.";
    }
  } else {
    LOG(INFO) << "Could not find any TTY devices.";
  }
  globfree(&paths);

  for (std::map<std::string, bool>::const_iterator it = files.begin();
      it != files.end(); ++it) {
    ASSERT_DEATH({
        if (!freopen(it->first.c_str(), "w", stdout)) {
          _exit(1);
        }
        if (Console::IsColoredOutputSupported() == it->second) {
          std::cerr << passed_message << std::endl;
          _exit(1);
        }
      }, passed_message);
  }
}

TEST(Console, ColoredPrintf) {
  if (!Console::IsColoredOutputSupported()) {
    std::cout << "Colored output is not supported." << std::endl;
  }
  const char* const color_names[] = { "black", "red", "green", "orange", "blue",
      "magenta", "cyan", "white", "default" };
  for (size_t i = 0; i < arraysize(color_names); ++i) {
    Console::Color color = static_cast<Console::Color>(i);
    Console::ColoredPrintf(color, "This should be %s.\n", color_names[i]);
  }
}

TEST(Console, ClearScreen) {
  Console::ClearScreen();
}

}  // anonymous namespace
}  // namespace base
