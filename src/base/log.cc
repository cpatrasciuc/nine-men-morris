// Copyright (c) 2012 Cristian Patrasciuc. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "base/log.h"

#include <string.h>
#include <sys/types.h>
#include <time.h>
#include <unistd.h>

#include <cerrno>
#include <cstdlib>
#include <iostream>
#include <sstream>
#include <string>

#include "base/debug/stacktrace.h"
#include "base/threading/lock.h"
#include "base/threading/thread.h"

namespace base {

LogLevel Log::max_log_level = DEBUG;

std::ostream* Log::default_output_stream = &std::cerr;

inline std::string LogLevelToString(LogLevel level) {
  switch (level) {
    case ERROR:
      return "ERROR";
    case WARNING:
      return "WARNING";
    case INFO:
      return "INFO";
    case DEBUG:
      return "DEBUG";
  }
  NOTREACHED();
  return std::string();
}

std::string GetCurrentTime() {
  time_t rawtime;
  time(&rawtime);
  const size_t kMaxBufferSize = 20;
  char buffer[kMaxBufferSize];
  tm time_info;
  localtime_r(&rawtime, &time_info);
  size_t len = strftime(buffer, kMaxBufferSize, "%b %d %X", &time_info);
  if (len) {
    return std::string(buffer, len);
  }
  return std::string();
}

base::threading::Lock LogMessage::log_lock(new base::threading::MutexLockImpl);

LogMessage::LogMessage(LogLevel level,
                       Location location,
                       std::ostream& stream)
    : stream_(stream), log_guard_(&LogMessage::log_lock) {
  PrintHeader(level, location);
}

LogMessage::~LogMessage() {
  stream_ << std::endl;
}

void LogMessage::PrintHeader(LogLevel level, Location location) {
  stream_ << "[";
  stream_ << LogLevelToString(level);
  stream_ << "][";
  stream_ << GetCurrentTime();
  stream_ << "][";
  stream_ << getpid();
  stream_ << "][";
  if (location.thread()) {
    stream_ << location.thread()->name();
  } else {
    stream_ << "Main";
  }
  stream_ << "][" << location.file_name().value() << "("
          << location.line_number() << ")] ";
}

SystemErrorLogMessage::SystemErrorLogMessage(LogLevel level, Location location,
    std::ostream& stream)
  : LogMessage(level, location, stream) {}

SystemErrorLogMessage::~SystemErrorLogMessage() {
  stream() << ": ";
  const int kMaxBufferSize = 256;
  char buffer[kMaxBufferSize];
#if (_POSIX_C_SOURCE >= 200112L || _XOPEN_SOURCE >= 600) && !_GNU_SOURCE
  int result = strerror_r(errno, buffer, kMaxBufferSize);
  if (result == -1) {
    stream() << "Could not obtain the system error message (errno:"
             << errno << ")";
  } else {
    stream() << buffer;
  }
  }
#else
  char *result = strerror_r(errno, buffer, kMaxBufferSize);
  stream() << result;
#endif
}

AssertionFailedLogMessage::AssertionFailedLogMessage(LogLevel level,
    Location location, std::ostream& stream)
    : LogMessage(level, location, stream) {}

AssertionFailedLogMessage::~AssertionFailedLogMessage() {
  stream() << std::endl;
  base::debug::PrintStackTrace(32, &stream());
  stream() << std::endl;
  std::exit(1);
}

}  // namespace base
