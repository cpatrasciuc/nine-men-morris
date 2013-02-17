// Copyright (c) 2012 Cristian Patrasciuc. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include <memory>

#include "base/debug/stacktrace.h"
#include "game/game_options.h"
#include "console_game/console_game.h"
#include "console_game/human_player.h"
#include "console_game/player.h"

using console_game::HumanPlayer;
using console_game::Player;

int main(int argc, char **argv) {
  ::base::debug::EnableStackTraceDumpOnCrash();
  game::GameOptions options;
  HumanPlayer* white_player(new HumanPlayer("White Player"));
  white_player->SetupDefaultCommandHandlers();
  HumanPlayer* black_player(new HumanPlayer("Black Player"));
  black_player->SetupDefaultCommandHandlers();
  console_game::ConsoleGame current_game(options,
      std::auto_ptr<Player>(white_player), std::auto_ptr<Player>(black_player));
  current_game.Run();
  return 0;
}
