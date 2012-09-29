// Copyright (c) 2012 Cristian Patrasciuc. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include <sys/types.h>
#include <unistd.h>

#include <algorithm>
#include <cmath>
#include <limits>
#include <sstream>
#include <string>

#include "base/log.h"
#include "base/basic_macros.h"
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
  LogUnittest() {}
  ~LogUnittest() {}

  virtual void SetUp() {
    old_stream_ = Log::default_output_stream;
    old_log_level_ = Log::max_log_level;
    Log::default_output_stream = &test_stream_;
    Log::max_log_level = DEBUG;
  }

  virtual void TearDown() {
    Log::default_output_stream = old_stream_;
    Log::max_log_level = old_log_level_;
  }

  std::ostringstream& test_stream() {
    return test_stream_;
  }

 private:
  std::ostringstream test_stream_;
  std::ostream* old_stream_;
  LogLevel old_log_level_;

  DISALLOW_COPY_AND_ASSIGN(LogUnittest);
};

TEST_F(LogUnittest, MAYBE(LogLevelFilteringTest)) {
  LogLevel levels[] = {ERROR, WARNING, INFO, DEBUG};
  for (unsigned int level = 0; level < arraysize(levels); ++level) {
    test_stream().str("");  // Clear the buffer
    Log::max_log_level = levels[level];
    LOG(ERROR)   << "Error message";
    LOG(WARNING) << "Warning message";
    LOG(INFO)    << "Info message";
    LOG(DEBUG)   << "Debug message";
    std::string output = test_stream().str();
    const unsigned int lines_count =
        std::count(output.begin(), output.end(), '\n');
    EXPECT_EQ(level + 1, lines_count);
  }
}

TEST_F(LogUnittest, MAYBE(LogIfTest)) {
  LOG_IF(INFO, 0 == 1) << "Should not be logged";
  EXPECT_EQ(std::string(), test_stream().str());
  std::string message("Should be logged");
  LOG_IF(INFO, 0 != 1) << message;
  EXPECT_NE(std::string::npos, test_stream().str().find(message));
}

TEST_F(LogUnittest, LogMessageFormatTest) {
  std::string source_file("foobar.cc");
  std::string text_message("Foo Bar");
  {
    LogMessage log_message(INFO, 69, source_file.c_str(), test_stream());
    log_message.stream() << text_message;
  }
  std::string output = test_stream().str();
  EXPECT_NE(std::string::npos, output.find("INFO"));
  EXPECT_NE(std::string::npos, output.find("69"));
  EXPECT_NE(std::string::npos, output.find(source_file));
  EXPECT_NE(std::string::npos, output.find(text_message));
}

TEST_F(LogUnittest, MAYBE(SystemErrorLogIfTest)) {
  float result = std::pow(10.0f, 2.0f);
  ELOG_IF(ERROR, result == HUGE_VAL) << "Should not be logged";
  EXPECT_EQ(std::string(), test_stream().str());

  std::string message("Should be logged");
  float max_float = std::numeric_limits<float>::max();
  result = std::pow(max_float, max_float);
  ELOG_IF(ERROR, result == HUGE_VAL) << message;
  std::string output = test_stream().str();
  size_t pos = output.find(message);
  EXPECT_NE(std::string::npos, pos);
  EXPECT_LT(pos, output.size() - message.size());
}

TEST_F(LogUnittest, MAYBE(DCHECKPass)) {
  Log::max_log_level = ERROR;
  DCHECK_LT(0, 1) << "Should not be logged";
  EXPECT_EQ(std::string(), test_stream().str());
}

// This will intentionally crash.
// Don't enable it unless you are testing the DCHECK macro behavior.
TEST(LogUnittestDeathTest, DCHECKFail) {
  Log::max_log_level = ERROR;
  Log::default_output_stream = &std::cerr;
  std::string crash_message("DCHECK failed as expected");
  ASSERT_DEATH(DCHECK_GT(0, 1) << crash_message, crash_message);
}

}  // anonymous namespace
}  // namespace base
