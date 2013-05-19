// Copyright (c) 2013 Cristian Patrasciuc. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "ai/alphabeta/evaluators.h"

#include <vector>

#include "game/board.h"
#include "game/board_location.h"

namespace ai {
namespace alphabeta {

int Mobility(const game::Board& board, game::PieceColor player) {
  int result = 0;
  const std::vector<game::BoardLocation>& locations = board.locations();
  for (size_t i = 0; i < locations.size(); ++i) {
    const game::PieceColor color = board.GetPieceAt(locations[i]);
    if (color != game::NO_COLOR) {
      std::vector<game::BoardLocation> adjacent_locations;
      board.GetAdjacentLocations(locations[i], &adjacent_locations);
      for (size_t i = 0; i < adjacent_locations.size(); ++i) {
        if (board.GetPieceAt(adjacent_locations[i])) {
          result += int(board.GetPieceAt(locations[i]) == player);
        }
      }
    }
  }
  return result;
}

int Material(const game::Board& board, game::PieceColor player) {
  return board.GetPieceCountByColor(player);
}

int Mills(const game::Board& board, game::PieceColor player) {
  int result = 0;
  const std::vector<game::BoardLocation>& locations = board.locations();
  for (size_t i = 0; i < locations.size(); ++i) {
    if (board.IsPartOfMill(locations[i])) {
      result += int(board.GetPieceAt(locations[i]) == player);
    }
  }
  return result;
}

int OpponentEval(Evaluator* evaluator,
                 const game::Board& board,
                 game::PieceColor player) {
  return (*evaluator)(board, game::GetOpponent(player));
}

}  // namespace alphabeta
}  // namespace ai
