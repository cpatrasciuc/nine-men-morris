// Copyright (c) 2012 Cristian Patrasciuc. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef BASE_CONSOLE_H_
#define BASE_CONSOLE_H_

#include "base/base_export.h"
#include "base/basic_macros.h"

namespace base {

class BASE_EXPORT Console {
 public:
  // Constants that can be used for text color (i.e. foreground color)
  enum Color {
    COLOR_BLACK,
    COLOR_RED,
    COLOR_GREEN,
    COLOR_ORANGE,
    COLOR_BLUE,
    COLOR_MAGENTA,
    COLOR_CYAN,
    COLOR_WHITE,
    COLOR_DEFAULT
  };

  // Checks if the standard output supports colors (i.e. if the output file
  // descriptor is associated to a TTY).
  static bool IsColoredOutputSupported();

  // Similar with printf() with an additional color argument. If colored output
  // is supported, the message will be printed with the given color. Otherwise,
  // the message will be printed with the default terminal color.
  static void ColoredPrintf(Color color, const char* fmt, ...);

  // Equivalent of running the system "clear" command.
  static void ClearScreen();

 private:
  DISALLOW_COPY_AND_ASSIGN(Console);
};

}  // namespace base

#endif  // BASE_CONSOLE_H_

