// Copyright (c) 2012 Cristian Patrasciuc. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include <cstdlib>
#include <iostream>
#include <memory>

#include "base/command_line.h"
#include "base/debug/stacktrace.h"
#include "game/game_options.h"
#include "console_game/console_game.h"
#include "console_game/human_player.h"
#include "console_game/player.h"

using console_game::HumanPlayer;
using console_game::Player;

namespace {

const char kGameTypeSwitch[] = "--game-type";
const char kHelpSwitch[] = "--help";

void Usage() {
  std::cout << "Posible command line options:" << std::endl;
  std::cout << "\t" << kGameTypeSwitch << "=3|6|9" << std::endl;
  std::cout << "\t\t" << "Specifies the game type: three/six/nine men morris;"
            << std::endl;
  std::cout << "\t" << kHelpSwitch << std::endl;
  std::cout << "\t\t" << "Displays this help message and exits." << std::endl;
}

bool RunConsoleGame(const base::CommandLine& cmd_line) {
  game::GameOptions options;
  if (cmd_line.HasSwitch(kGameTypeSwitch)) {
    const std::string game_type(cmd_line.GetSwitchValue(kGameTypeSwitch));
    if (game_type == "3") {
      options.set_game_type(game::GameOptions::THREE_MEN_MORRIS);
    } else if (game_type == "6") {
      options.set_game_type(game::GameOptions::SIX_MEN_MORRIS);
    } else if (game_type == "9") {
      options.set_game_type(game::GameOptions::NINE_MEN_MORRIS);
    } else {
      Usage();
      return false;
    }
  }
  HumanPlayer* white_player(new HumanPlayer("White Player"));
  white_player->SetupDefaultCommandHandlers();
  HumanPlayer* black_player(new HumanPlayer("Black Player"));
  black_player->SetupDefaultCommandHandlers();
  console_game::ConsoleGame current_game(options,
      std::auto_ptr<Player>(white_player), std::auto_ptr<Player>(black_player));
  current_game.Run();
  return true;
}

}  // anonymous namespace

int main(int argc, char** argv) {
  base::debug::EnableStackTraceDumpOnCrash();
  base::CommandLine* cmd_line = base::CommandLine::ForCurrentProcess();
  cmd_line->Init(argc, argv);
  bool result = true;
  if (cmd_line->HasSwitch(kHelpSwitch)) {
    Usage();
  } else {
    result = RunConsoleGame(*cmd_line);
  }
  base::CommandLine::DeleteForCurrentProcess();
  return result ? EXIT_SUCCESS : EXIT_FAILURE;
}
