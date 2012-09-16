// Copyright (c) 2012 Cristian Patrasciuc. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "base/log.h"

#include <string.h>
#include <sys/types.h>
#include <time.h>
#include <unistd.h>

#include <cerrno>
#include <iostream>
#include <sstream>
#include <string>

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

LogMessage::LogMessage(LogLevel level,
                       int line_number,
                       const char* file_name,
                       std::ostream& stream)
    : stream_(stream) {
  PrintHeader(level, line_number, file_name);
}

LogMessage::~LogMessage() {
  stream_ << std::endl;
}

void LogMessage::PrintHeader(LogLevel level, int line_number,
                             const char* file_name) {
  stream_ << "[";
  stream_ << LogLevelToString(level);
  stream_ << "][";
  stream_ << GetCurrentTime();
  stream_ << "][";
  stream_ << getpid();
  // TODO(threading): Add thread ID
  // TODO(file_util): Add a FilePath class or file_util module
  stream_ << "][" << basename(file_name) << "(" << line_number << ")] ";
}

SystemErrorLogMessage::SystemErrorLogMessage(LogLevel level, int line_number,
    const char* file_name, std::ostream& stream)
  : LogMessage(level, line_number, file_name, stream) {}

SystemErrorLogMessage::~SystemErrorLogMessage() {
  stream() << ": ";
  const int kMaxBufferSize = 256;
  char buffer[256];
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

}  // namespace base
