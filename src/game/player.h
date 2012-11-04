// Copyright (c) 2012 Cristian Patrasciuc. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef GAME_PLAYER_H_
#define GAME_PLAYER_H_

#include <string>

#include "base/basic_macros.h"
#include "game/board.h"
#include "game/game_export.h"
#include "game/player_action.h"

namespace game {

class Game;

class GAME_EXPORT Player {
 public:
  explicit Player(const std::string& name);
  virtual ~Player();

  const std::string& name() const { return name_; }

  Board::PieceColor color() const { return color_; }
  void set_color(const Board::PieceColor color) { color_ = color; }

  void set_game(Game* game) { game_ = game; }

  // TODO(player): Remove the temporary implementation
  virtual PlayerAction* GetNextAction(PlayerAction::ActionType type) {
    return NULL;
  };

 private:
  std::string name_;
  Board::PieceColor color_;
  Game* game_;

  DISALLOW_COPY_AND_ASSIGN(Player);
};

}  // namespace game

#endif  // GAME_PLAYER_H_

