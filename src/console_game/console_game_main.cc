// Copyright (c) 2012 Cristian Patrasciuc. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "base/debug/stacktrace.h"
#include "console_game/console_game.h"

int main(int argc, char **argv) {
  ::base::debug::EnableStackTraceDumpOnCrash();
  console_game::ConsoleGame game;
  game.Run();
  return 0;
}
