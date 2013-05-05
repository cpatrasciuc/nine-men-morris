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
  for (int line = 0; line < board.size(); ++line) {
    for (int col = 0; col < board.size(); ++col) {
      const game::BoardLocation loc(line, col);
      if (!board.IsValidLocation(loc)) {
        continue;
      }
      const game::PieceColor color = board.GetPieceAt(loc);
      if (color != game::NO_COLOR) {
        std::vector<game::BoardLocation> adjacent_locations;
        board.GetAdjacentLocations(loc, &adjacent_locations);
        for (size_t i = 0; i < adjacent_locations.size(); ++i) {
          if (board.GetPieceAt(adjacent_locations[i])) {
            result += board.GetPieceAt(loc) == player ? 1 : -1;
          }
        }
      }
    }
  }
  return result;
}

int Material(const game::Board& board, game::PieceColor player) {
  const game::PieceColor other = game::GetOpponent(player);
  return board.GetPieceCountByColor(player) - board.GetPieceCountByColor(other);
}

int Mills(const game::Board& board, game::PieceColor player) {
  int result = 0;
  for (int line = 0; line < board.size(); ++line) {
    for (int col = 0; col < board.size(); ++col) {
      const game::BoardLocation loc(line, col);
      if (!board.IsValidLocation(loc)) {
        continue;
      }
      if (board.IsPartOfMill(loc)) {
        result += board.GetPieceAt(loc) == player ? 1 : -1;
      }
    }
  }
  return result;
}

}  // namespace alphabeta
}  // namespace ai
