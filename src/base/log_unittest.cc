// Copyright (c) 2012 Cristian Patrasciuc. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include <sys/types.h>
#include <unistd.h>

#include <algorithm>
#include <sstream>
#include <string>

#include "base/log.h"
#include "gtest/gtest.h"

namespace base {
namespace {

#ifdef ENABLE_LOGGING
#define MAYBE(testcase) testcase
#else
#define MAYBE(testcase) DISABLED_##testcase
#endif

class LogUnittest : public ::testing::Test {
 public:
  virtual void SetUp() {
    old_stream_ = Log::default_output_stream;
    old_log_level_ = Log::max_log_level;
  }

  virtual void TearDown() {
    Log::default_output_stream = old_stream_;
    Log::max_log_level = old_log_level_;
  }

 private:
  std::ostream* old_stream_;
  LogLevel old_log_level_;
};

TEST_F(LogUnittest, MAYBE(LogLevelFilteringTest)) {
  LogLevel levels[] = {ERROR, WARNING, INFO, DEBUG};
  // TODO: add arraysize macro
  for (int test_level = 0; test_level <= DEBUG; ++test_level) {
    std::ostringstream test_stream;
    Log::default_output_stream = &test_stream;
    Log::max_log_level = levels[test_level];
    LOG(ERROR)   << "Error message";
    LOG(WARNING) << "Warning message";
    LOG(INFO)    << "Info message";
    LOG(DEBUG)   << "Debug message";
    std::string output = test_stream.str();
    const int lines_count =
        std::count(output.begin(), output.end(), '\n');
    EXPECT_EQ(test_level + 1, lines_count);
  }
}

TEST_F(LogUnittest, MAYBE(LogIfTest)) {
  std::ostringstream test_stream;
  Log::default_output_stream = &test_stream;
  Log::max_log_level = DEBUG;
  LOG_IF(INFO, 0 == 1) << "Should not be logged";
  EXPECT_EQ(std::string(), test_stream.str());
  std::string message("Should be logged");
  LOG_IF(INFO, 0 != 1) << message;
  EXPECT_NE(std::string::npos, test_stream.str().find(message));
}

TEST_F(LogUnittest, LogMessageFormatTest) {
  std::ostringstream test_stream;
  std::string source_file("foobar.cc");
  std::string text_message("Foo Bar");
  {
    LogMessage log_message(INFO, 69, source_file.c_str(), test_stream);
    log_message.stream() << text_message;
  }
  std::string output = test_stream.str();
  EXPECT_NE(std::string::npos, output.find("INFO"));
  EXPECT_NE(std::string::npos, output.find("69"));
  EXPECT_NE(std::string::npos, output.find(source_file));
  EXPECT_NE(std::string::npos, output.find(text_message));
}

}  // anonymous namespace
}  // namespace base
