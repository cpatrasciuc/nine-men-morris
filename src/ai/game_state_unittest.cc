// Copyright (c) 2013 Cristian Patrasciuc. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "ai/game_state.h"
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

TEST(GameStateDeathTest, PiecesInHand) {
  ::testing::FLAGS_gtest_death_test_style = "threadsafe";
  GameState state;
  ASSERT_DEATH(state.pieces_in_hand(game::NO_COLOR), "");
  ASSERT_DEATH(state.set_pieces_in_hand(game::NO_COLOR, 0), "");
  ASSERT_DEATH(state.set_pieces_in_hand(game::WHITE_COLOR, 100), "");
}

}  // anonymous namespace
}  // namespace ai
