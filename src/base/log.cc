// Copyright (c) 2012 Cristian Patrasciuc. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "base/log.h"

#include <sys/types.h>
#include <time.h>
#include <unistd.h>

#include <iostream>
#include <sstream>
#include <string>

namespace base {

LogLevel Log::max_log_level = DEBUG;

std::ostream* Log::default_output_stream = &std::cerr;

inline std::string LogLevelToString(LogLevel log_level) {
  switch (log_level) {
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

LogMessage::LogMessage(LogLevel log_level,
                       int line_number,
                       const char* file_name,
                       std::ostream& stream)
    : stream_(stream) {
  PrintHeader(log_level, line_number, file_name);
}

LogMessage::~LogMessage() {
  stream_ << std::endl;
}

void LogMessage::PrintHeader(LogLevel log_level, int line_number,
                             const char* file_name) {
  stream_ << "[";
  stream_ << LogLevelToString(log_level);
  stream_ << "][";
  stream_ << GetCurrentTime();
  stream_ << "][";
  stream_ << getpid();
  // TODO(cpatrasciuc): Add thread ID
  stream_ << "][" << file_name << "(" << line_number << ")] ";
}
}
