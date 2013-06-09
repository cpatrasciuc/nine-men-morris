// Copyright (c) 2013 Cristian Patrasciuc. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include <vector>

#include "ai/game_state.h"
#include "base/basic_macros.h"
#include "game/board.h"
#include "game/board_location.h"
#include "game/game_type.h"
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
  EXPECT_TRUE((first < second) != (second < first));
  first = second;
  EXPECT_TRUE(first == second);
  EXPECT_FALSE((first < second) != (second < first));
  first.set_pieces_in_hand(game::BLACK_COLOR, 10);
  EXPECT_FALSE(first == second);
  EXPECT_TRUE((first < second) != (second < first));
  second.set_pieces_in_hand(game::BLACK_COLOR, 10);
  EXPECT_TRUE(first == second);
  EXPECT_FALSE((first < second) != (second < first));
  EXPECT_TRUE(first == first);
  EXPECT_FALSE((first < first) != (first < first));
  first = first;
  EXPECT_TRUE(first == second);
  EXPECT_FALSE((first < second) != (second < first));
}

class GameStateTest : public ::testing::TestWithParam<game::GameType> {
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
  const game::GameType game_type = GetParam();
  game::Board board(game_type);
  SetUpTestBoard(&board);
  GameState state(game_type);
  state.Encode(board);
  game::Board decoded_board(game_type);
  GameState copied_state(state);
  copied_state.Decode(&decoded_board);
  const std::vector<game::BoardLocation>& locs = board.locations();
  for (size_t i = 0; i < locs.size(); ++i) {
    EXPECT_EQ(board.GetPieceAt(locs[i]),
              decoded_board.GetPieceAt(locs[i])) << locs[i];
  }
}

TEST_P(GameStateTest, GetPlayerActionsSimplePlace) {
  const game::GameType game_type = GetParam();
  game::Board board(game_type);
  SetUpTestBoard(&board);
  GameState from(game_type);
  from.set_current_player(game::WHITE_COLOR);
  from.set_pieces_in_hand(game::WHITE_COLOR, 3);
  from.Encode(board);
  const game::BoardLocation destination(0, board.size() - 1);
  EXPECT_TRUE(board.AddPiece(destination, game::WHITE_COLOR));
  GameState to(game_type);
  to.set_current_player(game::BLACK_COLOR);
  to.set_pieces_in_hand(game::WHITE_COLOR, 2);
  to.Encode(board);
  const std::vector<game::PlayerAction> actions =
      GameState::GetTransition(from, to);
  EXPECT_EQ(1U, actions.size());
  EXPECT_EQ(game::PlayerAction::PLACE_PIECE, actions[0].type());
  EXPECT_EQ(destination, actions[0].destination());
  EXPECT_EQ(game::WHITE_COLOR, actions[0].player_color());
}

TEST_P(GameStateTest, GetPlayerActionsPlaceWithMill) {
  const game::GameType game_type = GetParam();
  game::Board board(game_type);
  SetUpTestBoard(&board);
  GameState from(game_type);
  from.set_current_player(game::WHITE_COLOR);
  from.set_pieces_in_hand(game::WHITE_COLOR, 3);
  from.Encode(board);
  const game::BoardLocation destination(board.size() - 1, 0);
  const game::BoardLocation remove_loc(board.size() - 1, board.size() - 1);
  EXPECT_TRUE(board.AddPiece(destination, game::WHITE_COLOR));
  EXPECT_TRUE(board.RemovePiece(remove_loc));
  GameState to(game_type);
  to.set_current_player(game::BLACK_COLOR);
  to.set_pieces_in_hand(game::WHITE_COLOR, 2);
  to.Encode(board);
  const std::vector<game::PlayerAction> actions =
      GameState::GetTransition(from, to);
  EXPECT_EQ(2U, actions.size());
  EXPECT_EQ(game::PlayerAction::PLACE_PIECE, actions[0].type());
  EXPECT_EQ(destination, actions[0].destination());
  EXPECT_EQ(game::WHITE_COLOR, actions[0].player_color());
  EXPECT_EQ(game::PlayerAction::REMOVE_PIECE, actions[1].type());
  EXPECT_EQ(remove_loc, actions[1].source());
  EXPECT_EQ(game::WHITE_COLOR, actions[1].player_color());
}

TEST_P(GameStateTest, GetPlayerActionsSimpleMove) {
  const game::GameType game_type = GetParam();
  game::Board board(game_type);
  SetUpTestBoard(&board);
  GameState from(game_type);
  from.set_current_player(game::WHITE_COLOR);
  from.Encode(board);
  const game::BoardLocation source(0, 0);
  const game::BoardLocation destination(board.size() - 1, 0);
  board.MovePiece(source, destination);
  GameState to(game_type);
  to.set_current_player(game::BLACK_COLOR);
  to.Encode(board);
  const std::vector<game::PlayerAction> actions =
      GameState::GetTransition(from, to);
  EXPECT_EQ(1U, actions.size());
  EXPECT_EQ(game::PlayerAction::MOVE_PIECE, actions[0].type());
  EXPECT_EQ(source, actions[0].source());
  EXPECT_EQ(destination, actions[0].destination());
  EXPECT_EQ(game::WHITE_COLOR, actions[0].player_color());
}

TEST_P(GameStateTest, GetPlayerActionsMoveWithMill) {
  const game::GameType game_type = GetParam();
  game::Board board(game_type);
  SetUpTestBoard(&board);
  GameState from(game_type);
  from.set_current_player(game::WHITE_COLOR);
  from.Encode(board);
  const game::BoardLocation source(0, 0);
  const game::BoardLocation destination(board.size() - 1, 0);
  const game::BoardLocation remove_loc(board.size() - 1, board.size() - 1);
  board.MovePiece(source, destination);
  board.RemovePiece(remove_loc);
  GameState to(game_type);
  to.set_current_player(game::BLACK_COLOR);
  to.Encode(board);
  const std::vector<game::PlayerAction> actions =
      GameState::GetTransition(from, to);
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
                        ::testing::Values(game::THREE_MEN_MORRIS,
                                          game::SIX_MEN_MORRIS,
                                          game::NINE_MEN_MORRIS));

TEST(GameStateDeathTest, DEBUG_ONLY_TEST(PiecesInHand)) {
  ::testing::FLAGS_gtest_death_test_style = "threadsafe";
  GameState state;
  ASSERT_DEATH(state.pieces_in_hand(game::NO_COLOR), "");
  ASSERT_DEATH(state.set_pieces_in_hand(game::NO_COLOR, 0), "");
  ASSERT_DEATH(state.set_pieces_in_hand(game::WHITE_COLOR, 100), "");
}

}  // anonymous namespace
}  // namespace ai
