// Copyright (c) 2012 Cristian Patrasciuc. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "base/debug/stacktrace.h"
#include "game/game_options.h"
#include "console_game/console_game.h"

int main(int argc, char **argv) {
  ::base::debug::EnableStackTraceDumpOnCrash();
  game::GameOptions options;
  console_game::ConsoleGame current_game(options);
  current_game.Run();
  return 0;
}
