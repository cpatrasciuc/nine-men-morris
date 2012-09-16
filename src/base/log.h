// Copyright (c) 2012 Cristian Patrasciuc. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef BASE_LOG_H_
#define BASE_LOG_H_

#include <iostream>
#include <ostream>

// This file provides the logging functionality through a set of macros.
// You can use constructs like the following throughout the source code
// to generate logging statements.
// Example:
//
//   LOG(INFO) << "This is some useful info";
//   LOG_IF(ERROR, p == NULL) << "The pointer is NULL";
//
// The output for these examples would be something similar to:
//
//   [INFO][Sep 16 16:16:27][9513][source_file.cc(10)] This is some useful info
//   [ERROR][Sep 16 16:16:27][9513][source_file.cc(11)] The pointer is NULL
//
// It is also possible to call a succession of insertions operations as:
//
//    LOG(INFO) << "The value of foo is " << foo << ".";
//
// If ENABLE_LOGGING is not defined, all log statements are optimized out at
// compile time.

// This is defined outside the namespace to avoid a using directive in all files
// that use logging functionality.
enum LogLevel {
  ERROR,
  WARNING,
  INFO,
  DEBUG
};

namespace base {

// Contains global logging settings
class Log {
 public:
  // Every logging statement with a level greater than this, is discarded
  static LogLevel max_log_level;

  // The default output stream to which LogMessage's are sent,
  // if no specific stream is specified at instantiation.
  static std::ostream* default_output_stream;
};

// Represents
class LogMessage {
 public:
  LogMessage(LogLevel log_level,
          int line_number,
          const char* file_name,
          std::ostream& stream = *Log::default_output_stream);
  ~LogMessage();

  std::ostream& stream() const {
    return stream_;
  }

 private:
  void PrintHeader(LogLevel log_level, int line_number, const char* file_name);
  std::ostream& stream_;
};

#ifdef ENABLE_LOGGING
#define LOG_IF(level, cond) \
  if ((level > base::Log::max_log_level) || !(cond)) \
    ; \
  else \
    base::LogMessage(level, __LINE__, __FILE__).stream()
#else
#define LOG_IF(level, cond) if (false) std::cerr
#endif

#define LOG(level) LOG_IF(level, true)

}  // namespace

#endif  // BASE_LOG_H_
