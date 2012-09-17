// Copyright (c) 2012 Cristian Patrasciuc. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "base/debug/stacktrace.h"

#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>
#include <wait.h>

#include "base/log.h"

namespace base {
namespace debug {

// http://neugierig.org/software/blog/2012/06/backtraces.html
void PrintStackTrace() {
  pid_t parent_pid = getpid();
  pid_t child_pid = fork();
  if (child_pid < 0) {
    ELOG(ERROR) << "fork() while collecting stack trace";
  } else if (child_pid == 0) {
    const int kMaxBufferSize = 128;
    char gdb_command[kMaxBufferSize];
    int result = snprintf(gdb_command,
        kMaxBufferSize,
        "gdb -p %d -batch -ex bt 2>/dev/null | sed '0,/%s/d'",
        parent_pid,
        __FUNCTION__);
    if (result > kMaxBufferSize) {
      LOG(ERROR) << "Buffer too small. Consider a size of at least " << result;
    } else {
      const char* argv[] = { "sh", "-c", gdb_command, NULL };
      execve("/bin/sh", const_cast<char**>(argv), NULL);
      _exit(1);
    }
  } else {
    int status;
    int result = waitpid(child_pid, &status, 0);
    if (result < 0) {
      ELOG(ERROR) << "waitpid()";
    }
    if (!WIFEXITED(status)) {
      LOG(ERROR) << "gdb process did not exit normally";
    }
  }
}

}  // namespace debug
}  // namespace base

