// Copyright (c) 2013 Cristian Patrasciuc. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include <string>

#include "base/log.h"
#include "console_game/console_game.h"
#include "console_game/player.h"

namespace console_game {

Player::Player(const std::string& name)
    : name_(name), color_(game::NO_COLOR), current_game_(NULL) {}

Player::~Player() {}

void Player::Initialize(ConsoleGame* game_ptr, const game::PieceColor color) {
  DCHECK(game_ptr);
  DCHECK(color != game::NO_COLOR);
  DCHECK_EQ(current_game_, NULL);
  DCHECK_EQ(color_, game::NO_COLOR);
  color_ = color;
  current_game_ = game_ptr;
}

}  // namespace console_game
