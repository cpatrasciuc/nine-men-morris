// Copyright (c) 2013 Cristian Patrasciuc. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "ai/game_state.h"

#include <stdint.h>

#include <bitset>

#include "base/log.h"
#include "game/board.h"
#include "game/piece_color.h"

namespace ai {

namespace {

const size_t kPlayerColorPos = 0;

}  // anonymous namespace

GameState::GameState() : s_() {}

GameState::GameState(const GameState& other) : s_(other.s_) {}

GameState::~GameState() {}

game::PieceColor GameState::current_player() const {
  return s_.test(kPlayerColorPos) ? game::BLACK_COLOR : game::WHITE_COLOR;
}

void GameState::set_current_player(const game::PieceColor player_color) {
  s_.set(kPlayerColorPos, player_color == game::BLACK_COLOR);
}

int GameState::pieces_in_hand(const game::PieceColor player_color) const {
  DCHECK(player_color != game::NO_COLOR);
  const uint64_t state = s_.to_ulong();
  if (player_color == game::WHITE_COLOR) {
    return ((state & 0x1eULL) >> 1);
  }
  return ((state & 0x1e0ULL) >> 5);
}

void GameState::set_pieces_in_hand(const game::PieceColor player_color,
                                   const int count) {
  DCHECK(player_color != game::NO_COLOR);
  DCHECK_LT(count, 1 << 5);
  uint64_t bitmask = static_cast<uint64_t>(count) << 1;
  if (player_color == game::BLACK_COLOR) {
    bitmask <<= 4;
  }
  s_ |= bitmask;
}

void GameState::Encode(const game::Board& board) {}

void GameState::Decode(game::Board* board) const {}

}  // namespace ai
