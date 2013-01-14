// Copyright (c) 2012 Cristian Patrasciuc. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "base/console.h"

#include <unistd.h>

#include <cstdarg>
#include <cstdio>
#include <string>

#include "base/string_util.h"

namespace base {

// static
bool Console::IsColoredOutputSupported() {
  return isatty(STDOUT_FILENO);
}

// static
void Console::ColoredPrintf(Console::Color color, const char* fmt, ...) {
  bool use_colors = (color != COLOR_DEFAULT) && IsColoredOutputSupported();
  if (use_colors) {
    std::string color_code = base::ToString(static_cast<int>(color) + 30);
    std::printf("\033[0;%sm", color_code.c_str());
  }
  va_list args;
  va_start(args, fmt);
  std::vprintf(fmt, args);
  va_end(args);
  if (use_colors) {
    std::printf("\033[m");
  }
}

// static
void Console::ClearScreen() {
  std::printf("\033[2J");
}

}  // namespace base
