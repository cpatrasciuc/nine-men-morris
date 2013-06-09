// Copyright (c) 2012-2013 Cristian Patrasciuc. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include <cstdlib>
#include <iostream>
#include <memory>

#include "ai/ai_algorithm.h"
#include "ai/alphabeta/morris_alphabeta.h"
#include "ai/random/random_algorithm.h"
#include "base/command_line.h"
#include "base/debug/stacktrace.h"
#include "game/game_options.h"
#include "game/game_type.h"
#include "console_game/ai_player.h"
#include "console_game/console_game.h"
#include "console_game/human_player.h"
#include "console_game/player.h"

namespace {

using console_game::AIPlayer;
using console_game::HumanPlayer;
using console_game::Player;

const char kGameTypeSwitch[] = "--game-type";
const char kWhitePlayerType[] = "--white-player";
const char kBlackPlayerType[] = "--black-player";
const char kHelpSwitch[] = "--help";

void Usage() {
  std::cout << "Possible command line options:" << std::endl;
  std::cout << "\t" << kGameTypeSwitch << "=3|6|9" << std::endl;
  std::cout << "\t\t" << "Specifies the game type: three/six/nine men morris."
            << std::endl;
  std::cout << "\t" << kWhitePlayerType << "=human|random|alphabeta"
            << std::endl;
  std::cout << "\t\t" << "Specifies the player type for the white color. "
            << "Default: human." << std::endl;
  std::cout << "\t" << kBlackPlayerType << "=human|random|alphabeta"
            << std::endl;
  std::cout << "\t\t" << "Specifies the player type for the black color. "
            << "Default: random (i.e. AI with RandomAlgorithm)." << std::endl;
  std::cout << "\t" << kHelpSwitch << std::endl;
  std::cout << "\t\t" << "Displays this help message and exits." << std::endl;
}

std::auto_ptr<Player> GetPlayerFromCmdLine(const base::CommandLine& cmd_line,
                                           const std::string& switch_name,
                                           const std::string& default_type,
                                           const game::GameOptions& options) {
  std::string player_type = default_type;
  if (cmd_line.HasSwitch(switch_name)) {
    player_type = cmd_line.GetSwitchValue(switch_name);
  }
  Player* player = NULL;
  if (player_type == "human") {
    HumanPlayer* human_player = new HumanPlayer("Human Player");
    human_player->SetupDefaultCommandHandlers();
    player = human_player;
  } else if (player_type == "random") {
    player = new AIPlayer("RandomAI",
        std::auto_ptr<ai::AIAlgorithm>(new ai::random::RandomAlgorithm()));
  } else if (player_type == "alphabeta") {
    player = new AIPlayer("AlphaBeta", std::auto_ptr<ai::AIAlgorithm>(
        new ai::alphabeta::MorrisAlphaBeta(options)));
  }
  return std::auto_ptr<Player>(player);
}

bool RunConsoleGame(const base::CommandLine& cmd_line) {
  game::GameOptions options;
  if (cmd_line.HasSwitch(kGameTypeSwitch)) {
    const std::string game_type(cmd_line.GetSwitchValue(kGameTypeSwitch));
    if (game_type == "3") {
      options.set_game_type(game::THREE_MEN_MORRIS);
    } else if (game_type == "6") {
      options.set_game_type(game::SIX_MEN_MORRIS);
    } else if (game_type == "9") {
      options.set_game_type(game::NINE_MEN_MORRIS);
    } else {
      Usage();
      return false;
    }
  }
  std::auto_ptr<Player> white_player(
      GetPlayerFromCmdLine(cmd_line, kWhitePlayerType, "human", options));
  std::auto_ptr<Player> black_player(
      GetPlayerFromCmdLine(cmd_line, kBlackPlayerType, "alphabeta", options));
  if (!white_player.get() || !black_player.get()) {
    Usage();
    return false;
  }
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
