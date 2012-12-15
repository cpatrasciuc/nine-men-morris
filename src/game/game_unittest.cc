// Copyright (c) 2012 Cristian Patrasciuc. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include <memory>
#include <sstream>
#include <string>

#include "game/board.h"
#include "game/board_location.h"
#include "game/game.h"
#include "game/game_test_helper.h"
#include "game/player_action.h"
#include "gtest/gtest.h"

namespace game {
namespace {

TEST(GameTest, ThreeMenMorris) {
  std::auto_ptr<Game> game = LoadSavedGameForTests("full_3");
  ASSERT_TRUE(game.get());
  EXPECT_TRUE(game->is_game_over());
  EXPECT_EQ(Board::BLACK_COLOR, game->winner());
}

TEST(GameTest, SixMenMorris) {
  std::auto_ptr<Game> game = LoadSavedGameForTests("full_6");
  ASSERT_TRUE(game.get());
  EXPECT_TRUE(game->is_game_over());
  EXPECT_EQ(Board::WHITE_COLOR, game->winner());
}

TEST(GameTest, BlackStarts) {
  GameOptions options;
  options.set_white_starts(false);
  Game game(options);
  game.Initialize();
  PlayerAction invalid_action(Board::WHITE_COLOR, PlayerAction::PLACE_PIECE);
  invalid_action.set_destination(BoardLocation(0, 0));
  EXPECT_FALSE(game.CanExecutePlayerAction(invalid_action));
  PlayerAction valid_action(Board::BLACK_COLOR, PlayerAction::PLACE_PIECE);
  valid_action.set_destination(BoardLocation(0, 0));
  EXPECT_TRUE(game.CanExecutePlayerAction(valid_action));
}

}  // anonymous namespace
}  // namespace game

