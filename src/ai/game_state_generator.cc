// Copyright (c) 2013 Cristian Patrasciuc. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "ai/game_state_generator.h"

#include <vector>

#include "ai/game_state.h"
#include "base/log.h"
#include "game/board.h"
#include "game/game_options.h"
#include "game/piece_color.h"

namespace ai {

GameStateGenerator::GameStateGenerator(game::GameOptions::GameType game_type)
    : game_type_(game_type) {}

void GameStateGenerator::GetSuccessors(const GameState& state,
                                       std::vector<GameState>* successors) {
  if (state.pieces_in_hand(state.current_player()) > 0) {
    GetPlaceSuccessors(state, successors);
  }
}

void GameStateGenerator::GetPlaceSuccessors(const GameState& state,
    std::vector<GameState>* successors) {
  game::Board board(game_type_);
  state.Decode(&board);
  const game::PieceColor player = state.current_player();
  const game::PieceColor opponent = game::GetOpponent(player);
  for (int line = 0; line < board.size(); ++line) {
    for (int col = 0; col < board.size(); ++col) {
      const game::BoardLocation loc(line, col);
      if (!board.IsValidLocation(loc)) {
        continue;
      }
      if (board.GetPieceAt(loc) != game::NO_COLOR) {
        continue;
      }
      bool result = board.AddPiece(loc, player);
      if (result) {
        GameState successor;
        successor.set_current_player(opponent);
        successor.set_pieces_in_hand(player, state.pieces_in_hand(player) - 1);
        successor.set_pieces_in_hand(opponent, state.pieces_in_hand(opponent));
        successor.Encode(board);
        successors->push_back(successor);
      } else {
        NOTREACHED();
      }
      result = board.RemovePiece(loc);
      DCHECK(result);
    }
  }
}



}  // namespace ai
