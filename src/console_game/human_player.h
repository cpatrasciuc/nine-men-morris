// Copyright (c) 2013 Cristian Patrasciuc. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef CONSOLE_GAME_HUMAN_PLAYER_H_
#define CONSOLE_GAME_HUMAN_PLAYER_H_

#include <string>

#include "base/basic_macros.h"
#include "console_game/player.h"

namespace game {
class Game;
class PlayerAction;
}

namespace console_game {

class HumanPlayer : public Player {
 public:
  HumanPlayer(const std::string& name, const game::PieceColor& color);
  virtual ~HumanPlayer();

  virtual game::PlayerAction GetNextAction(const game::Game& game);

 private:
  DISALLOW_COPY_AND_ASSIGN(HumanPlayer);
};

}  // namespace console_game

#endif  // CONSOLE_GAME_HUMAN_PLAYER_H_

