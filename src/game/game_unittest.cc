// Copyright (c) 2012 Cristian Patrasciuc. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include <memory>
#include <sstream>
#include <string>

#include "game/board.h"
#include "game/board_location.h"
#include "game/game.h"
#include "game/game_serializer.h"
#include "game/player_action.h"
#include "gtest/gtest.h"

namespace game {
namespace {

TEST(GameTest, ThreeMenMorris) {
  const std::string game_string =
      "48 "
      "21 "
      "PLACE WHITE 2 2 "
      "PLACE BLACK 0 1 "
      "PLACE WHITE 1 1 "
      "PLACE BLACK 0 2 "
      "PLACE WHITE 0 0 "
      "PLACE BLACK 2 0 "
      "MOVE WHITE 0 0 1 0 "
      "MOVE BLACK 0 2 1 2 "
      "MOVE WHITE 2 2 2 1 "
      "MOVE BLACK 0 1 0 0 "
      "MOVE WHITE 1 1 0 1 "
      "MOVE BLACK 1 2 1 1 "
      "MOVE WHITE 2 1 2 2 "
      "MOVE BLACK 1 1 1 2 "
      "MOVE WHITE 0 1 0 2 "
      "MOVE BLACK 2 0 2 1 "
      "MOVE WHITE 1 0 2 0 "
      "MOVE BLACK 0 0 0 1 "
      "MOVE WHITE 2 0 1 0 "
      "MOVE BLACK 1 2 1 1 "
      "REMOVE BLACK 1 0 ";
  std::istringstream in(game_string);
  std::auto_ptr<Game> game = GameSerializer::DeserializeFrom(&in, false);
  ASSERT_TRUE(game.get());
  EXPECT_TRUE(game->is_game_over());
  EXPECT_EQ(Board::BLACK_COLOR, game->winner());
}

TEST(GameTest, SixMenMorris) {
  // http://www.youtube.com/watch?v=GErHm21qbsk
  const std::string game_string =
      "49 "
      "66 "
      "PLACE WHITE 3 1 "
      "PLACE BLACK 2 0 "
      "PLACE WHITE 4 2 "
      "PLACE BLACK 0 2 "
      "PLACE WHITE 4 4 "
      "PLACE BLACK 4 0 "
      "PLACE WHITE 0 0 "
      "PLACE BLACK 3 2 "
      "PLACE WHITE 2 3 "
      "PLACE BLACK 2 4 "
      "PLACE WHITE 0 4 "
      "PLACE BLACK 2 1 "
      "MOVE WHITE 2 3 3 3 "
      "MOVE BLACK 2 4 2 3 "
      "MOVE WHITE 4 4 2 4 "
      "MOVE BLACK 2 1 1 1 "
      "MOVE WHITE 4 2 4 4 "
      "REMOVE WHITE 2 3 "
      "MOVE BLACK 1 1 1 2 "
      "MOVE WHITE 4 4 4 2 "
      "MOVE BLACK 1 2 1 3 "
      "MOVE WHITE 4 2 4 4 "
      "REMOVE WHITE 0 2 "
      "MOVE BLACK 1 3 2 3 "
      "MOVE WHITE 4 4 4 2 "
      "MOVE BLACK 2 0 2 1 "
      "MOVE WHITE 4 2 4 4 "
      "REMOVE WHITE 2 3 "
      "MOVE BLACK 3 2 4 2 "
      "MOVE WHITE 3 3 2 3 "
      "MOVE BLACK 4 0 3 3 "
      "MOVE WHITE 2 3 1 3 "
      "MOVE BLACK 3 3 2 3 "
      "MOVE WHITE 1 3 1 2 "
      "MOVE BLACK 2 1 0 2 "
      "MOVE WHITE 3 1 3 2 "
      "MOVE BLACK 4 2 2 0 "
      "MOVE WHITE 3 2 4 2 "
      "MOVE BLACK 2 0 3 2 "
      "MOVE WHITE 0 0 2 0 "
      "MOVE BLACK 0 2 4 0 "
      "MOVE WHITE 1 2 0 2 "
      "MOVE BLACK 3 2 0 0 "
      "MOVE WHITE 0 2 1 2 "
      "MOVE BLACK 0 0 3 2 "
      "MOVE WHITE 2 0 0 0 "
      "MOVE BLACK 3 2 0 2 "
      "MOVE WHITE 0 0 2 0 "
      "MOVE BLACK 0 2 1 3 "
      "MOVE WHITE 2 0 0 0 "
      "MOVE BLACK 4 0 3 3 "
      "REMOVE BLACK 1 2 "
      "MOVE WHITE 0 0 0 2 "
      "MOVE BLACK 1 3 3 2 "
      "MOVE WHITE 0 2 1 2 "
      "MOVE BLACK 3 2 1 3 "
      "REMOVE BLACK 1 2 "
      "MOVE WHITE 4 2 3 2 "
      "MOVE BLACK 1 3 2 1 "
      "MOVE WHITE 3 2 4 2 "
      "MOVE BLACK 2 1 1 3 "
      "REMOVE BLACK 4 2 "
      "MOVE WHITE 4 4 0 2 "
      "MOVE BLACK 2 3 4 2 "
      "MOVE WHITE 0 2 4 4 "
      "REMOVE WHITE 3 3 ";
  std::istringstream in(game_string);
  std::auto_ptr<Game> game = GameSerializer::DeserializeFrom(&in, false);
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

