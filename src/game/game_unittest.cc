// Copyright (c) 2012 Cristian Patrasciuc. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include <memory>
#include <sstream>
#include <string>

#include "game/board.h"
#include "game/game.h"
#include "game/game_serializer.h"
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

}  // anonymous namespace
}  // namespace game

