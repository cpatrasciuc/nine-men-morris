// Copyright (c) 2013 Cristian Patrasciuc. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "ai/game_state.h"
#include "base/basic_macros.h"
#include "game/board.h"
#include "game/board_location.h"
#include "game/game_options.h"
#include "game/piece_color.h"
#include "gtest/gtest.h"

namespace ai {
namespace {

TEST(GameState, PiecesInHand) {
  GameState state;
  EXPECT_EQ(0, state.pieces_in_hand(game::WHITE_COLOR));
  EXPECT_EQ(0, state.pieces_in_hand(game::BLACK_COLOR));
  state.set_pieces_in_hand(game::WHITE_COLOR, 5);
  state.set_pieces_in_hand(game::BLACK_COLOR, 9);
  EXPECT_EQ(5, state.pieces_in_hand(game::WHITE_COLOR));
  EXPECT_EQ(9, state.pieces_in_hand(game::BLACK_COLOR));
}

class GameStateCodecTest
    : public ::testing::TestWithParam<game::GameOptions::GameType> {
};

TEST_P(GameStateCodecTest, EncodeAndDecode) {
  const game::GameOptions::GameType game_type = GetParam();
  game::Board board(game_type);
  const int size = board.size();
  EXPECT_TRUE(board.AddPiece(game::BoardLocation(0, 0),
                             game::WHITE_COLOR));
  EXPECT_TRUE(board.AddPiece(game::BoardLocation(size / 2, 0),
                             game::WHITE_COLOR));
  EXPECT_TRUE(board.AddPiece(game::BoardLocation(0, size / 2),
                             game::BLACK_COLOR));
  EXPECT_TRUE(board.AddPiece(game::BoardLocation(size - 1, size - 1),
                             game::BLACK_COLOR));
  GameState state;
  state.Encode(board);
  game::Board decoded_board(game_type);
  state.Decode(&decoded_board);
  for (int line = 0; line < board.size(); ++line) {
    for (int col = 0; col < board.size(); ++col) {
      const game::BoardLocation loc(line, col);
      if (decoded_board.IsValidLocation(loc)) {
        EXPECT_EQ(board.GetPieceAt(loc), decoded_board.GetPieceAt(loc)) << loc;
      }
    }
  }
}

INSTANTIATE_TEST_CASE_P(GameStateCodecTestInstance,
                        GameStateCodecTest,
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
