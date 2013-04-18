// Copyright (c) 2013 Cristian Patrasciuc. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "ai/game_state.h"
#include "ai/game_state_generator.h"
#include "game/board.h"
#include "game/board_location.h"
// TODO(game_type): Extract the GameType enum in a separate header.
#include "game/game_options.h"
#include "gtest/gtest.h"

namespace ai {
namespace {

TEST(GameStateGenerator, Place) {
  // TODO(game_state): Run this over all board sizes
  const game::GameOptions game_options;
  game::Board board(game_options.game_type());
  GameState start;
  start.set_pieces_in_hand(game::WHITE_COLOR, 9);
  start.set_pieces_in_hand(game::BLACK_COLOR, 9);
  start.set_current_player(game::WHITE_COLOR);
  start.Encode(board);
  std::vector<GameState> successors;
  GameStateGenerator generator(game_options);
  generator.GetSuccessors(start, &successors);
  unsigned int valid_locations_count = 0U;
  for (int line = 0; line < board.size(); ++line) {
    for (int col = 0; col < board.size(); ++col) {
      if (board.IsValidLocation(game::BoardLocation(line, col))) {
        ++valid_locations_count;
      }
    }
  }
  EXPECT_EQ(valid_locations_count, successors.size());
  GameState second(successors[0]);
  EXPECT_EQ(game::BLACK_COLOR, second.current_player());
  EXPECT_EQ(8, second.pieces_in_hand(game::WHITE_COLOR));
  EXPECT_EQ(9, second.pieces_in_hand(game::BLACK_COLOR));
  successors.clear();
  generator.GetSuccessors(second, &successors);
  EXPECT_EQ(valid_locations_count - 1, successors.size());
}

TEST(GameStateGenerator, SimpleMove) {
  game::GameOptions game_options;
  game_options.set_jumps_allowed(false);
  game::Board board(game_options.game_type());
  GameState start;
  start.set_current_player(game::WHITE_COLOR);
  game::BoardLocation piece_location(0, 0);
  board.AddPiece(piece_location, game::WHITE_COLOR);
  start.Encode(board);
  GameStateGenerator generator(game_options);
  std::vector<GameState> successors;
  generator.GetSuccessors(start, &successors);
  std::vector<game::BoardLocation> adjacent_locations;
  board.GetAdjacentLocations(piece_location, &adjacent_locations);
  EXPECT_EQ(adjacent_locations.size(), successors.size());
}

TEST(GameStateGenerator, Jumps) {
  game::GameOptions game_options;
  game::Board board(game_options.game_type());
  GameState start;
  start.set_current_player(game::WHITE_COLOR);
  game::BoardLocation piece_location(0, 0);
  board.AddPiece(piece_location, game::WHITE_COLOR);
  start.Encode(board);
  GameStateGenerator generator(game_options);
  std::vector<GameState> successors;
  generator.GetSuccessors(start, &successors);
  // TODO(game_state): Refactor the duplicated code across test
  // TODO(game_state): Write unit tests for moves that close a mill
  unsigned int valid_locations_count = 0U;
  for (int line = 0; line < board.size(); ++line) {
    for (int col = 0; col < board.size(); ++col) {
      if (board.IsValidLocation(game::BoardLocation(line, col))) {
        ++valid_locations_count;
      }
    }
  }
  EXPECT_EQ(valid_locations_count - 1, successors.size());
}

}  // anonymous namespace
}  // namespace ai

