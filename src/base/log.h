// Copyright (c) 2012 Cristian Patrasciuc. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef BASE_LOG_H_
#define BASE_LOG_H_

#include <iostream>
#include <ostream>

#include "base/base_export.h"
#include "base/basic_macros.h"
#include "base/location.h"
#include "base/threading/scoped_guard.h"

namespace base {
namespace threading {
class Lock;
}
}

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
class BASE_EXPORT Log {
 public:
  // Every logging statement with a level greater than this, is discarded
  static LogLevel max_log_level;

  // The default output stream to which LogMessage's are sent,
  // if no specific stream is specified at instantiation.
  static std::ostream* default_output_stream;

 private:
  DISALLOW_COPY_AND_ASSIGN(Log);
};

// Handles one logging statement. It prints a header including the log level,
// current time, process ID, thread ID, source file name and line. Any data
// can then be appended using the stream's operator<<.
// The whole content is flushed to the output stream as soon as the LogMessage
// instance gets out of scope.
class BASE_EXPORT LogMessage {
 public:
  LogMessage(LogLevel level,
             Location location,
             std::ostream& stream = *Log::default_output_stream);
  ~LogMessage();

  std::ostream& stream() const {
    return stream_;
  }

 private:
  void PrintHeader(LogLevel level, Location location);
  std::ostream& stream_;

  // Shared lock by all log messages to avoid mixing out the output from
  // multiple threads.
  static base::threading::Lock log_lock;

  // Guard used to acquire the shared |log_lock| such that no one else writes to
  // the output until we are done.
  // TODO(threading): Check if this lock here is really OK for performance
  base::threading::ScopedGuard log_guard_;

  DISALLOW_COPY_AND_ASSIGN(LogMessage);
};

class BASE_EXPORT SystemErrorLogMessage : public LogMessage {
 public:
  SystemErrorLogMessage(LogLevel level,
                        Location location,
                        std::ostream& stream = *Log::default_output_stream);
  ~SystemErrorLogMessage();

 private:
  DISALLOW_COPY_AND_ASSIGN(SystemErrorLogMessage);
};

class BASE_EXPORT AssertionFailedLogMessage : public LogMessage {
 public:
  AssertionFailedLogMessage(LogLevel level,
      Location location,
      std::ostream& stream = *Log::default_output_stream);
  ~AssertionFailedLogMessage();

 private:
  DISALLOW_COPY_AND_ASSIGN(AssertionFailedLogMessage);
};

#define EAT_LOG_STATEMENT(condition) if (false && (condition)) std::cerr

#ifdef ENABLE_LOGGING
#define LOG_TEMPLATE(level, condition) \
  if ((level > base::Log::max_log_level) || !(condition)) \
    ; \
  else
#define LOG_IF(level, condition) LOG_TEMPLATE(level, condition) \
  base::LogMessage(level, FROM_HERE).stream()
#define ELOG_IF(level, condition) LOG_TEMPLATE(level, condition) \
  base::SystemErrorLogMessage(level, FROM_HERE).stream()
#else
#define LOG_IF(level, condition) EAT_LOG_STATEMENT(condition)
#define ELOG_IF(level, condition) EAT_LOG_STATEMENT(condition)
#endif

#ifdef ENABLE_DCHECK
#define DCHECK(condition) LOG_TEMPLATE(ERROR, !(condition)) \
  base::AssertionFailedLogMessage(ERROR, FROM_HERE).stream()
#else
#define DCHECK(condition) EAT_LOG_STATEMENT(condition) << ""
#endif

#define LOG(level) LOG_IF(level, true)
#define ELOG(level) ELOG_IF(level, true)

#define DCHECK_LT(a, b) DCHECK((a) < (b))
#define DCHECK_GT(a, b) DCHECK((a) > (b))
#define DCHECK_EQ(a, b) DCHECK((a) == (b))

#define NOTREACHED() DCHECK(false)

}  // namespace base

#endif  // BASE_LOG_H_
