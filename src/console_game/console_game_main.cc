// Copyright (c) 2012 Cristian Patrasciuc. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include <memory>

#include "base/debug/stacktrace.h"
#include "game/game_options.h"
#include "console_game/command_handler.h"
#include "console_game/console_game.h"
#include "console_game/save_game_command_handler.h"

int main(int argc, char **argv) {
  ::base::debug::EnableStackTraceDumpOnCrash();
  game::GameOptions options;
  console_game::ConsoleGame current_game(options);
  std::auto_ptr<console_game::CommandHandler> save_game_handler(
      new console_game::SaveGameCommandHandler);
  current_game.AddCommandHandler(
      console_game::SaveGameCommandHandler::kSaveGameCommandName,
      save_game_handler);
  current_game.Run();
  return 0;
}
