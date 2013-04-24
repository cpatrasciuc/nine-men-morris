// Copyright (c) 2012 Cristian Patrasciuc. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include <set>
#include <vector>

#include "base/basic_macros.h"
#include "game/board.h"
#include "game/game_type.h"
#include "game/piece_color.h"
#include "game/player_action.h"
#include "gtest/gtest.h"

namespace game {
namespace {

TEST(PlayerAction, AddRemovePiece) {
  Board board;
  BoardLocation loc(0, 0);
  PlayerAction action(WHITE_COLOR, PlayerAction::PLACE_PIECE);
  action.set_destination(loc);
  EXPECT_EQ(0, board.piece_count());
  EXPECT_EQ(NO_COLOR, board.GetPieceAt(loc));
  EXPECT_EQ(0, board.GetPieceCountByColor(action.player_color()));
  action.Execute(&board);
  EXPECT_EQ(1, board.piece_count());
  EXPECT_EQ(action.player_color(), board.GetPieceAt(loc));
  EXPECT_EQ(1, board.GetPieceCountByColor(action.player_color()));

  std::vector<BoardLocation> v;
  board.GetAdjacentLocations(loc, &v);
  for (int i = 0; i < static_cast<int>(v.size()); ++i) {
    PlayerAction opponent_action(BLACK_COLOR, PlayerAction::PLACE_PIECE);
    opponent_action.set_destination(v[i]);
    EXPECT_EQ(i, board.GetPieceCountByColor(opponent_action.player_color()));
    opponent_action.Execute(&board);
    EXPECT_EQ(i+1, board.GetPieceCountByColor(opponent_action.player_color()));
  }

  for (int i = static_cast<int>(v.size()) - 1; i >= 0; --i) {
    PlayerAction remove_action(WHITE_COLOR, PlayerAction::REMOVE_PIECE);
    remove_action.set_source(v[i]);
    EXPECT_EQ(i + 1, board.GetPieceCountByColor(BLACK_COLOR));
    remove_action.Execute(&board);
    EXPECT_EQ(i, board.GetPieceCountByColor(BLACK_COLOR));
  }
}

TEST(PlayerAction, MovePiece) {
  std::set<BoardLocation> visited;
  Board board;
  BoardLocation loc(0, 0);
  PieceColor color = BLACK_COLOR;
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

TEST(PlayerAction, Undo) {
  // We place a piece, move it and remove it from the board, then we undo each
  // action (in reverse order).
  Board board;
  PieceColor color = WHITE_COLOR;
  PlayerAction place_piece(color, PlayerAction::PLACE_PIECE);
  place_piece.set_destination(BoardLocation(0, 0));
  place_piece.Execute(&board);
  EXPECT_EQ(color, board.GetPieceAt(place_piece.destination()));
  EXPECT_EQ(1, board.piece_count());
  PlayerAction move_action(color, PlayerAction::MOVE_PIECE);
  move_action.set_source(place_piece.destination());
  std::vector<BoardLocation> adjacent_locations;
  board.GetAdjacentLocations(move_action.source(), &adjacent_locations);
  move_action.set_destination(adjacent_locations[0]);
  move_action.Execute(&board);
  EXPECT_EQ(NO_COLOR, board.GetPieceAt(move_action.source()));
  EXPECT_EQ(color, board.GetPieceAt(move_action.destination()));
  EXPECT_EQ(1, board.piece_count());
  PlayerAction remove_piece(GetOpponent(color), PlayerAction::REMOVE_PIECE);
  remove_piece.set_source(move_action.destination());
  remove_piece.Execute(&board);
  EXPECT_EQ(NO_COLOR, board.GetPieceAt(remove_piece.source()));
  EXPECT_EQ(0, board.piece_count());

  remove_piece.Undo(&board);
  EXPECT_EQ(NO_COLOR, board.GetPieceAt(move_action.source()));
  EXPECT_EQ(color, board.GetPieceAt(move_action.destination()));
  EXPECT_EQ(1, board.piece_count());
  move_action.Undo(&board);
  EXPECT_EQ(color, board.GetPieceAt(place_piece.destination()));
  EXPECT_EQ(1, board.piece_count());
  place_piece.Undo(&board);
  EXPECT_EQ(0, board.piece_count());
}

TEST(PlayerAction, CanExecuteOn) {
  Board board;
  board.AddPiece(BoardLocation(0, 0), BLACK_COLOR);

  PlayerAction place_action(WHITE_COLOR, PlayerAction::PLACE_PIECE);
  // Already occupied location
  place_action.set_destination(BoardLocation(0, 0));
  EXPECT_FALSE(place_action.CanExecuteOn(board));
  // Invalid location
  place_action.set_destination(BoardLocation(0, 1));
  EXPECT_FALSE(place_action.CanExecuteOn(board));
  // Good
  place_action.set_destination(BoardLocation(0, 3));
  EXPECT_TRUE(place_action.CanExecuteOn(board));
  place_action.Execute(&board);

  PlayerAction move_action(BLACK_COLOR, PlayerAction::MOVE_PIECE);
  // Invalid source
  EXPECT_FALSE(move_action.CanExecuteOn(board));
  // Invalid destination
  move_action.set_source(BoardLocation(0, 0));
  EXPECT_FALSE(move_action.CanExecuteOn(board));
  // Invalid source color
  move_action.set_destination(BoardLocation(3, 0));
  move_action.set_source(place_action.destination());
  EXPECT_FALSE(move_action.CanExecuteOn(board));
  // Good
  move_action.set_source(BoardLocation(0, 0));
  EXPECT_TRUE(move_action.CanExecuteOn(board));
  // Destination is not empty
  move_action.set_destination(BoardLocation(0, 3));
  EXPECT_FALSE(move_action.CanExecuteOn(board));

  PlayerAction remove_action(BLACK_COLOR, PlayerAction::REMOVE_PIECE);
  // Invalid source
  EXPECT_FALSE(remove_action.CanExecuteOn(board));
  // Invalid source color
  remove_action.set_source(BoardLocation(0, 0));
  EXPECT_FALSE(remove_action.CanExecuteOn(board));
  // Good
  remove_action.set_source(BoardLocation(0, 3));
  EXPECT_TRUE(remove_action.CanExecuteOn(board));
}

TEST(PlayerAction, CanUndoFrom) {
  Board board;

  PlayerAction place_action(WHITE_COLOR, PlayerAction::PLACE_PIECE);
  // Invalid destination
  EXPECT_FALSE(place_action.CanUndoFrom(board));
  // Empty destination
  place_action.set_destination(BoardLocation(0, 0));
  EXPECT_FALSE(place_action.CanUndoFrom(board));
  // Good
  board.AddPiece(place_action.destination(), place_action.player_color());
  EXPECT_TRUE(place_action.CanUndoFrom(board));

  PlayerAction move_action(BLACK_COLOR, PlayerAction::MOVE_PIECE);
  // Invalid source
  EXPECT_FALSE(move_action.CanUndoFrom(board));
  // Invalid destination
  move_action.set_source(BoardLocation(0, 0));
  EXPECT_FALSE(move_action.CanUndoFrom(board));
  // Empty destination
  move_action.set_destination(BoardLocation(3, 0));
  move_action.set_source(BoardLocation(6, 0));
  EXPECT_FALSE(move_action.CanUndoFrom(board));
  // Good
  board.AddPiece(move_action.destination(), move_action.player_color());
  EXPECT_TRUE(move_action.CanUndoFrom(board));
  // Source is not empty
  move_action.set_source(place_action.destination());
  EXPECT_FALSE(move_action.CanUndoFrom(board));

  PlayerAction remove_action(BLACK_COLOR, PlayerAction::REMOVE_PIECE);
  // Invalid source
  EXPECT_FALSE(remove_action.CanUndoFrom(board));
  // Good
  remove_action.set_source(BoardLocation(6, 6));
  EXPECT_TRUE(remove_action.CanUndoFrom(board));
  // Non-empty source
  board.AddPiece(remove_action.source(), remove_action.player_color());
  EXPECT_FALSE(remove_action.CanUndoFrom(board));
}

TEST(PlayerAction, RemovePieceFromMill) {
  Board board;
  // Form a mill
  board.AddPiece(BoardLocation(0, 0), WHITE_COLOR);
  board.AddPiece(BoardLocation(3, 0), WHITE_COLOR);
  board.AddPiece(BoardLocation(6, 0), WHITE_COLOR);
  // Add a 'free' piece
  BoardLocation free_piece(6, 6);
  board.AddPiece(free_piece, WHITE_COLOR);

  PlayerAction remove_from_mill(BLACK_COLOR, PlayerAction::REMOVE_PIECE);
  remove_from_mill.set_source(BoardLocation(0, 0));
  EXPECT_FALSE(remove_from_mill.CanExecuteOn(board));
  board.RemovePiece(free_piece);
  EXPECT_TRUE(remove_from_mill.CanExecuteOn(board));
}

TEST(PlayerAction, IsJump) {
  Board board9(NINE_MEN_MORRIS);
  Board board3(THREE_MEN_MORRIS);

  PlayerAction move_action(WHITE_COLOR, PlayerAction::MOVE_PIECE);
  move_action.set_source(BoardLocation(0, 0));
  move_action.set_destination(BoardLocation(0, 1));
  EXPECT_FALSE(move_action.IsJumpOn(board3));
  EXPECT_FALSE(move_action.IsJumpOn(board9));
  move_action.set_destination(BoardLocation(0, 2));
  EXPECT_TRUE(move_action.IsJumpOn(board3));
  EXPECT_FALSE(move_action.IsJumpOn(board9));
  move_action.set_destination(BoardLocation(0, 3));
  EXPECT_FALSE(move_action.IsJumpOn(board3));
  EXPECT_FALSE(move_action.IsJumpOn(board9));
  move_action.set_destination(BoardLocation(0, 6));
  EXPECT_FALSE(move_action.IsJumpOn(board3));
  EXPECT_TRUE(move_action.IsJumpOn(board9));

  PlayerAction remove_action(WHITE_COLOR, PlayerAction::REMOVE_PIECE);
  remove_action.set_source(BoardLocation(0, 0));
  EXPECT_FALSE(remove_action.IsJumpOn(board3));
  EXPECT_FALSE(remove_action.IsJumpOn(board9));

  PlayerAction place_action(WHITE_COLOR, PlayerAction::PLACE_PIECE);
  place_action.set_destination(BoardLocation(0, 0));
  EXPECT_FALSE(place_action.IsJumpOn(board3));
  EXPECT_FALSE(place_action.IsJumpOn(board9));
}

TEST(PlayerActionDeathTest, DEBUG_ONLY_TEST(ExecuteOrUndoInvalidAction)) {
  ::testing::FLAGS_gtest_death_test_style = "threadsafe";
  Board board;
  PlayerAction remove_action(WHITE_COLOR, PlayerAction::REMOVE_PIECE);
  EXPECT_FALSE(remove_action.CanExecuteOn(board));
  ASSERT_DEATH(remove_action.Execute(&board), "");
  EXPECT_FALSE(remove_action.CanUndoFrom(board));
  ASSERT_DEATH(remove_action.Execute(&board), "");
}

TEST(PlayerActionDeathTest, DEBUG_ONLY_TEST(InvalidPlayerColor)) {
  ::testing::FLAGS_gtest_death_test_style = "threadsafe";
  ASSERT_DEATH(PlayerAction(NO_COLOR, PlayerAction::MOVE_PIECE), "");
}

}  // anonymous namespace
}  // namespace game
