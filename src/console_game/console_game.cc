// Copyright (c) 2012 Cristian Patrasciuc. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "console_game/console_game.h"

#include <memory>
#include <string>

#include "base/console.h"
#include "base/log.h"
#include "console_game/player.h"
#include "game/game_options.h"
#include "game/piece_color.h"

namespace console_game {

ConsoleGame::ConsoleGame(const game::GameOptions& options,
                         std::auto_ptr<Player> white_player,
                         std::auto_ptr<Player> black_player)
    : game_(options),
      board_renderer_(game_.board()),
      white_player_(white_player.release()),
      black_player_(black_player.release()),
      should_quit_(false) {
  white_player_->Initialize(this, game::WHITE_COLOR);
  black_player_->Initialize(this, game::BLACK_COLOR);
}

ConsoleGame::~ConsoleGame() {}

void ConsoleGame::Draw() {
  base::Console::ClearScreen();
  board_renderer_.Render();
}

void ConsoleGame::Run() {
  should_quit_ = false;
  game_.Initialize();
  std::string last_command_status("Game started");
  do {
    Draw();
    std::cout << "\n\n";
    base::Console::ColoredPrintf(base::Console::COLOR_WHITE,
                                 last_command_status.c_str());
    std::cout << "\n\n";
    if (game_.is_game_over()) {
      std::cout << (game_.winner() == game::WHITE_COLOR ? "Red" : "Green")
                << " player wins." << std::endl;
      break;
    }
    last_command_status =
        game_.current_player() == game::WHITE_COLOR ?
        white_player_->GetNextAction(&game_) :
        black_player_->GetNextAction(&game_);
  } while (!should_quit_);
}

}  // namespace console_game
