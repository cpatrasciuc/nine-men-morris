// Copyright (c) 2013 Cristian Patrasciuc. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef CONSOLE_GAME_PLAYER_H_
#define CONSOLE_GAME_PLAYER_H_

#include <string>

#include "base/basic_macros.h"
#include "game/piece_color.h"

namespace game {
class Game;
class PlayerAction;
}

namespace console_game {

class Player {
 public:
  Player(const std::string& name, const game::PieceColor& color);
  virtual ~Player();

  const std::string& name() const { return name_; }

  const game::PieceColor color() const { return color_; }

  virtual const game::PlayerAction& GetNextAction(const game::Game& game) = 0;

 private:
  const std::string name_;
  const game::PieceColor color_;

  DISALLOW_COPY_AND_ASSIGN(Player);
};

}  // namespace console_game

#endif  // CONSOLE_GAME_PLAYER_H_

