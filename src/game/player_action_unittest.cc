// Copyright (c) 2012 Cristian Patrasciuc. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include <set>
#include <vector>

#include "game/board.h"
#include "game/player_action.h"
#include "gtest/gtest.h"

namespace game {
namespace {

TEST(PlayerAction, AddRemovePiece) {
  Board board;
  BoardLocation loc(0, 0);
  PlayerAction action(Board::WHITE_COLOR, PlayerAction::PLACE_PIECE);
  action.set_destination(loc);
  EXPECT_EQ(0, board.piece_count());
  EXPECT_EQ(Board::NO_COLOR, board.GetPieceAt(loc));
  EXPECT_EQ(0, board.GetPieceCountByColor(action.player_color()));
  action.Execute(&board);
  EXPECT_EQ(1, board.piece_count());
  EXPECT_EQ(action.player_color(), board.GetPieceAt(loc));
  EXPECT_EQ(1, board.GetPieceCountByColor(action.player_color()));

  std::vector<BoardLocation> v;
  board.GetAdjacentLocations(loc, &v);
  for (int i = 0; i < static_cast<int>(v.size()); ++i) {
    PlayerAction opponent_action(Board::BLACK_COLOR, PlayerAction::PLACE_PIECE);
    opponent_action.set_destination(v[i]);
    EXPECT_EQ(i, board.GetPieceCountByColor(opponent_action.player_color()));
    opponent_action.Execute(&board);
    EXPECT_EQ(i+1, board.GetPieceCountByColor(opponent_action.player_color()));
  }

  for (int i = static_cast<int>(v.size()) - 1; i >= 0; --i) {
    PlayerAction remove_action(Board::WHITE_COLOR, PlayerAction::REMOVE_PIECE);
    remove_action.set_source(v[i]);
    EXPECT_EQ(i + 1, board.GetPieceCountByColor(Board::BLACK_COLOR));
    remove_action.Execute(&board);
    EXPECT_EQ(i, board.GetPieceCountByColor(Board::BLACK_COLOR));
  }
}

TEST(PlayerAction, MovePiece) {
  std::set<BoardLocation> visited;
  Board board;
  BoardLocation loc(0, 0);
  Board::PieceColor color = Board::BLACK_COLOR;
  board.AddPiece(loc, color);
  BoardLocation current = loc;
  bool has_nonvisited_adjacent_locations = true;
  while (has_nonvisited_adjacent_locations) {
    visited.insert(current);
    PlayerAction action(color, PlayerAction::MOVE_PIECE);
    action.set_source(current);
    std::vector<BoardLocation> adjacent_locations;
    board.GetAdjacentLocations(current, &adjacent_locations);
    has_nonvisited_adjacent_locations = false;
    for (size_t i = 0; i < adjacent_locations.size(); ++i) {
      if (!visited.count(adjacent_locations[i])) {
        action.set_destination(adjacent_locations[i]);
        action.Execute(&board);
        current = action.destination();
        has_nonvisited_adjacent_locations = true;
        break;
      }
    }
  }
}

}  // anonymous namespace
}  // namespace game
