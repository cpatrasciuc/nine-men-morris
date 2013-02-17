// Copyright (c) 2013 Cristian Patrasciuc. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef CONSOLE_GAME_PLAYER_H_
#define CONSOLE_GAME_PLAYER_H_

#include <string>

#include "base/basic_macros.h"
#include "console_game/console_game_export.h"
#include "game/piece_color.h"

namespace game {
class Game;
}

namespace console_game {

class ConsoleGame;

class CONSOLE_GAME_EXPORT Player {
 public:
  explicit Player(const std::string& name);
  virtual ~Player();

  const std::string& name() const { return name_; }

  const game::PieceColor color() const { return color_; }

  void set_current_game(ConsoleGame* game_ptr, const game::PieceColor color);
  ConsoleGame* current_game() const { return current_game_; }

  virtual std::string GetNextAction(game::Game* game_model) = 0;

 private:
  const std::string name_;
  game::PieceColor color_;
  ConsoleGame* current_game_;

  DISALLOW_COPY_AND_ASSIGN(Player);
};

}  // namespace console_game

#endif  // CONSOLE_GAME_PLAYER_H_

