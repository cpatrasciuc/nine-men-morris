// Copyright (c) 2013 Cristian Patrasciuc. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "ai/game_state.h"

#include <bitset>

#include "game/board.h"
#include "game/piece_color.h"

namespace ai {

GameState::GameState() : s_() {}

GameState::GameState(const GameState& other) : s_(other.s_) {}

GameState::~GameState() {}

game::PieceColor GameState::current_player() const {
  return game::WHITE_COLOR;
}

void GameState::set_current_player(const game::PieceColor player_color) {
}

int GameState::pieces_in_hand(const game::PieceColor player_color) const {
  return 0;
}

void GameState::set_pieces_in_hand(const game::PieceColor player_color,
                                   const int count) {
}

void GameState::Encode(const game::Board& board) {}

void GameState::Decode(game::Board* board) const {}

}  // namespace ai
