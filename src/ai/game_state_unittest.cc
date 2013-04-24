// Copyright (c) 2013 Cristian Patrasciuc. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "ai/game_state.h"
#include "base/basic_macros.h"
#include "game/board.h"
#include "game/board_location.h"
#include "game/game_options.h"
#include "game/piece_color.h"
#include "game/player_action.h"
#include "gtest/gtest.h"

namespace ai {
namespace {

TEST(GameState, CurrentPlayer) {
  GameState state;
  EXPECT_EQ(game::WHITE_COLOR, state.current_player());
  state.set_current_player(game::BLACK_COLOR);
  EXPECT_EQ(game::BLACK_COLOR, state.current_player());
  state.set_current_player(game::WHITE_COLOR);
  EXPECT_EQ(game::WHITE_COLOR, state.current_player());
}

TEST(GameState, PiecesInHand) {
  GameState state;
  EXPECT_EQ(0, state.pieces_in_hand(game::WHITE_COLOR));
  EXPECT_EQ(0, state.pieces_in_hand(game::BLACK_COLOR));
  state.set_pieces_in_hand(game::WHITE_COLOR, 5);
  state.set_pieces_in_hand(game::BLACK_COLOR, 9);
  EXPECT_EQ(5, state.pieces_in_hand(game::WHITE_COLOR));
  EXPECT_EQ(9, state.pieces_in_hand(game::BLACK_COLOR));
  state.set_pieces_in_hand(game::WHITE_COLOR, 0);
  state.set_pieces_in_hand(game::BLACK_COLOR, 0);
  EXPECT_EQ(0, state.pieces_in_hand(game::WHITE_COLOR));
  EXPECT_EQ(0, state.pieces_in_hand(game::BLACK_COLOR));
}

TEST(GameState, Operators) {
  GameState first;
  first.set_current_player(game::WHITE_COLOR);
  GameState second;
  second.set_current_player(game::BLACK_COLOR);
  EXPECT_FALSE(first == second);
  first = second;
  EXPECT_TRUE(first == second);
  first.set_pieces_in_hand(game::BLACK_COLOR, 10);
  EXPECT_FALSE(first == second);
  second.set_pieces_in_hand(game::BLACK_COLOR, 10);
  EXPECT_TRUE(first == second);
  EXPECT_TRUE(first == first);
  first = first;
  EXPECT_TRUE(first == second);
}

class GameStateTest
    : public ::testing::TestWithParam<game::GameOptions::GameType> {
 protected:
  void SetUpTestBoard(game::Board* board) {
    const int size = board->size();
    EXPECT_TRUE(board->AddPiece(game::BoardLocation(0, 0),
                                game::WHITE_COLOR));
    EXPECT_TRUE(board->AddPiece(game::BoardLocation(size / 2, 0),
                                game::WHITE_COLOR));
    EXPECT_TRUE(board->AddPiece(game::BoardLocation(0, size / 2),
                                game::BLACK_COLOR));
    EXPECT_TRUE(board->AddPiece(game::BoardLocation(size - 1, size - 1),
                                game::BLACK_COLOR));
  }
};

TEST_P(GameStateTest, EncodeAndDecode) {
  const game::GameOptions::GameType game_type = GetParam();
  game::Board board(game_type);
  SetUpTestBoard(&board);
  GameState state;
  state.Encode(board);
  game::Board decoded_board(game_type);
  GameState copied_state(state);
  copied_state.Decode(&decoded_board);
  for (int line = 0; line < board.size(); ++line) {
    for (int col = 0; col < board.size(); ++col) {
      const game::BoardLocation loc(line, col);
      if (decoded_board.IsValidLocation(loc)) {
        EXPECT_EQ(board.GetPieceAt(loc), decoded_board.GetPieceAt(loc)) << loc;
      }
    }
  }
}

TEST_P(GameStateTest, GetPlayerActionsSimplePlace) {
  const game::GameOptions::GameType game_type = GetParam();
  game::Board board(game_type);
  SetUpTestBoard(&board);
  GameState from;
  from.set_current_player(game::WHITE_COLOR);
  from.set_pieces_in_hand(game::WHITE_COLOR, 3);
  from.Encode(board);
  const game::BoardLocation destination(0, board.size() - 1);
  EXPECT_TRUE(board.AddPiece(destination, game::WHITE_COLOR));
  GameState to;
  to.set_current_player(game::BLACK_COLOR);
  to.set_pieces_in_hand(game::WHITE_COLOR, 2);
  to.Encode(board);
  const std::vector<game::PlayerAction> actions =
      GameState::GetTransition(from, to, game_type);
  EXPECT_EQ(1U, actions.size());
  EXPECT_EQ(game::PlayerAction::PLACE_PIECE, actions[0].type());
  EXPECT_EQ(destination, actions[0].destination());
  EXPECT_EQ(game::WHITE_COLOR, actions[0].player_color());
}

TEST_P(GameStateTest, GetPlayerActionsPlaceWithMill) {
  const game::GameOptions::GameType game_type = GetParam();
  game::Board board(game_type);
  SetUpTestBoard(&board);
  GameState from;
  from.set_current_player(game::WHITE_COLOR);
  from.set_pieces_in_hand(game::WHITE_COLOR, 3);
  from.Encode(board);
  const game::BoardLocation destination(board.size() - 1, 0);
  const game::BoardLocation remove_loc(board.size() - 1, board.size() - 1);
  EXPECT_TRUE(board.AddPiece(destination, game::WHITE_COLOR));
  EXPECT_TRUE(board.RemovePiece(remove_loc));
  GameState to;
  to.set_current_player(game::BLACK_COLOR);
  to.set_pieces_in_hand(game::WHITE_COLOR, 2);
  to.Encode(board);
  const std::vector<game::PlayerAction> actions =
      GameState::GetTransition(from, to, game_type);
  EXPECT_EQ(2U, actions.size());
  EXPECT_EQ(game::PlayerAction::PLACE_PIECE, actions[0].type());
  EXPECT_EQ(destination, actions[0].destination());
  EXPECT_EQ(game::WHITE_COLOR, actions[0].player_color());
  EXPECT_EQ(game::PlayerAction::REMOVE_PIECE, actions[1].type());
  EXPECT_EQ(remove_loc, actions[1].source());
  EXPECT_EQ(game::WHITE_COLOR, actions[1].player_color());
}

TEST_P(GameStateTest, GetPlayerActionsSimpleMove) {
  const game::GameOptions::GameType game_type = GetParam();
  game::Board board(game_type);
  SetUpTestBoard(&board);
  GameState from;
  from.set_current_player(game::WHITE_COLOR);
  from.Encode(board);
  const game::BoardLocation source(0, 0);
  const game::BoardLocation destination(board.size() - 1, 0);
  board.MovePiece(source, destination);
  GameState to;
  to.set_current_player(game::BLACK_COLOR);
  to.Encode(board);
  const std::vector<game::PlayerAction> actions =
      GameState::GetTransition(from, to, game_type);
  EXPECT_EQ(1U, actions.size());
  EXPECT_EQ(game::PlayerAction::MOVE_PIECE, actions[0].type());
  EXPECT_EQ(source, actions[0].source());
  EXPECT_EQ(destination, actions[0].destination());
  EXPECT_EQ(game::WHITE_COLOR, actions[0].player_color());
}

TEST_P(GameStateTest, GetPlayerActionsMoveWithMill) {
  const game::GameOptions::GameType game_type = GetParam();
  game::Board board(game_type);
  SetUpTestBoard(&board);
  GameState from;
  from.set_current_player(game::WHITE_COLOR);
  from.Encode(board);
  const game::BoardLocation source(0, 0);
  const game::BoardLocation destination(board.size() - 1, 0);
  const game::BoardLocation remove_loc(board.size() - 1, board.size() - 1);
  board.MovePiece(source, destination);
  board.RemovePiece(remove_loc);
  GameState to;
  to.set_current_player(game::BLACK_COLOR);
  to.Encode(board);
  const std::vector<game::PlayerAction> actions =
      GameState::GetTransition(from, to, game_type);
  EXPECT_EQ(2U, actions.size());
  EXPECT_EQ(game::PlayerAction::MOVE_PIECE, actions[0].type());
  EXPECT_EQ(source, actions[0].source());
  EXPECT_EQ(destination, actions[0].destination());
  EXPECT_EQ(game::WHITE_COLOR, actions[0].player_color());
  EXPECT_EQ(game::PlayerAction::REMOVE_PIECE, actions[1].type());
  EXPECT_EQ(remove_loc, actions[1].source());
  EXPECT_EQ(game::WHITE_COLOR, actions[1].player_color());
}

INSTANTIATE_TEST_CASE_P(GameStateTestInstance,
                        GameStateTest,
                        ::testing::Values(game::GameOptions::THREE_MEN_MORRIS,
                                          game::GameOptions::SIX_MEN_MORRIS,
                                          game::GameOptions::NINE_MEN_MORRIS));

TEST(GameStateDeathTest, DEBUG_ONLY_TEST(PiecesInHand)) {
  ::testing::FLAGS_gtest_death_test_style = "threadsafe";
  GameState state;
  ASSERT_DEATH(state.pieces_in_hand(game::NO_COLOR), "");
  ASSERT_DEATH(state.set_pieces_in_hand(game::NO_COLOR, 0), "");
  ASSERT_DEATH(state.set_pieces_in_hand(game::WHITE_COLOR, 100), "");
}

}  // anonymous namespace
}  // namespace ai
