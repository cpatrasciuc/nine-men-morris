// Copyright (c) 2013 Cristian Patrasciuc. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include <string>

#include "console_game/human_player.h"
#include "console_game/player.h"
#include "game/player_action.h"

namespace console_game {

HumanPlayer::HumanPlayer(const std::string& name, const game::PieceColor& color)
    : Player(name, color) {}

HumanPlayer::~HumanPlayer() {}

game::PlayerAction HumanPlayer::GetNextAction(const game::Game& game) {
  return game::PlayerAction(color(), game::PlayerAction::MOVE_PIECE);
}

}  // namespace console_game
