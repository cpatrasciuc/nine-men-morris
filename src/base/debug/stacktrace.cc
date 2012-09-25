// Copyright (c) 2012 Cristian Patrasciuc. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "base/debug/stacktrace.h"

#include <cxxabi.h>
#include <execinfo.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>
#include <wait.h>
#include <sstream>
#include <string>

#include "base/basic_macros.h"
#include "base/log.h"
#include "base/ptr/scoped_array_ptr.h"
#include "base/ptr/scoped_malloc_ptr.h"

namespace base {
namespace debug {

using base::ptr::scoped_array_ptr;
using base::ptr::scoped_malloc_ptr;

// http://neugierig.org/software/blog/2012/06/backtraces.html
void DumpStackTraceForAllThreads() {
  const pid_t parent_pid = getpid();
  const pid_t child_pid = fork();
  if (child_pid < 0) {
    ELOG(ERROR) << "fork() while collecting stack trace";
  } else if (child_pid == 0) {
    const char gdb_command[] = "thread apply all bt";
    const int kMaxBufferSize = 128;
    char shell_command[kMaxBufferSize];
    const int result = snprintf(shell_command,
        kMaxBufferSize,
        "gdb -p %d -batch -ex '%s' 1>&2 2>/dev/null",
        parent_pid,
        gdb_command);
    if (result > kMaxBufferSize) {
      LOG(ERROR) << "Buffer too small. Consider a size of at least " << result;
    } else {
      const char* argv[] = { "sh", "-c", shell_command, NULL };
      execve("/bin/sh", const_cast<char**>(argv), NULL);
      ELOG(ERROR) << "execve() did not run successfully";
      _exit(1);
    }
  } else {
    int status;
    const int result = waitpid(child_pid, &status, 0);
    if (result < 0) {
      ELOG(ERROR) << "waitpid()";
    }
    if (!WIFEXITED(status)) {
      LOG(ERROR) << "gdb process did not exit normally";
    }
  }
}

void PrintStackTrace(const int max_depth, std::ostream* out) {
  scoped_array_ptr<void*> addresses(new void*[max_depth]);
  const int depth = backtrace(Get(addresses), max_depth);
  scoped_malloc_ptr<char*> function_names(
      backtrace_symbols(Get(addresses), depth));
  std::ostringstream result;
  if (function_names) {
    for (int i = 0; i < depth; ++i) {
      result << "#" << i << ": ";
      std::string function_line(function_names[i]);
      size_t start = function_line.find_last_of('(') + 1;
      if ((start != std::string::npos) && (start > 2)) {
        function_line.replace(0, start - 2,
          basename(function_line.substr(0, start - 2).c_str()));
        start = function_line.find_last_of('(') + 1;
      }
      const size_t end = function_line.find_last_of('+');
      if ((start != std::string::npos) &&
          (end != std::string::npos) &&
          (start < end)) {
        const std::string mangled = function_line.substr(start, end - start);
        int status = 0;  // will be -2 for C functions
        scoped_malloc_ptr<char> demangled(
            abi::__cxa_demangle(mangled.c_str(), NULL, 0, &status));
        if (demangled) {
          function_line.replace(start, mangled.length(), Get(demangled));
        } else if (status != -2) {
          result << "[demangle error: " << status << "] ";
        }
      } else if ((start != std::string::npos) && (end == std::string::npos)) {
        function_line.insert(start, "static?");
      }
      result << function_line << std::endl;
    }
    if (depth == max_depth) {
      result << "(...and more)" << std::endl;
    }
  }
  (*out) << result.str();
}

void DumpStackTraceAndExit(int signal) {
  DumpStackTraceForAllThreads();
  _exit(1);
}

void EnableStackTraceDumpOnCrash() {
  struct sigaction sa = {};
  sa.sa_handler = DumpStackTraceAndExit;
  const int signals[] = { SIGSEGV, SIGBUS, SIGILL };
  for (unsigned int i = 0; i < arraysize(signals); ++i) {
    int result = sigaction(signals[i], &sa, NULL);
    if (result < 0) {
      ELOG(ERROR) << "Could not install signal handler for " << signals[i];
    }
  }
}

}  // namespace debug
}  // namespace base

