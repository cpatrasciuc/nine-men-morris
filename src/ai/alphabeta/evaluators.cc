// Copyright (c) 2013 Cristian Patrasciuc. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "ai/alphabeta/evaluators.h"

#include <vector>

#include "game/board.h"
#include "game/board_location.h"
#include "game/piece_color.h"

namespace ai {
namespace alphabeta {

int Mobility(const game::Board& board, game::PieceColor player) {
  int result = 0;
  const std::vector<game::BoardLocation>& locations = board.locations();
  std::vector<game::BoardLocation> adjacent_locations;
  adjacent_locations.reserve(20);
  for (size_t i = 0; i < locations.size(); ++i) {
    if (board.GetPieceAt(locations[i]) == player) {
      board.GetAdjacentLocations(locations[i], &adjacent_locations);
    }
  }
  for (size_t i = 0; i < adjacent_locations.size(); ++i) {
    if (board.GetPieceAt(adjacent_locations[i]) == game::NO_COLOR) {
      ++result;
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
    if (board.GetPieceAt(locations[i]) == player) {
      result += int(board.IsPartOfMill(locations[i]));
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
