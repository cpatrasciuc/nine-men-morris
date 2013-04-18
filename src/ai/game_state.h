// Copyright (c) 2013 Cristian Patrasciuc. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef AI_GAME_STATE_H_
#define AI_GAME_STATE_H_

#include <bitset>

#include "ai/ai_export.h"
#include "game/piece_color.h"

namespace game {
class Board;
}

namespace ai {

class AI_EXPORT GameState {
 public:
  GameState();
  GameState(const GameState& other);
  ~GameState();

  game::PieceColor current_player() const;
  void set_current_player(const game::PieceColor player_color);

  int pieces_in_hand(const game::PieceColor player_color) const;
  void set_pieces_in_hand(const game::PieceColor player_color, const int count);

  void Encode(const game::Board& board);
  void Decode(game::Board* board) const;

  // TODO(game_state): Explicitly declare the assignment operator.

 private:
  std::bitset<64> s_;
};

}  // namespace ai

#endif  // AI_GAME_STATE_H_

