// Copyright (c) 2012 Cristian Patrasciuc. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include <sstream>
#include <string>
#include <vector>

#include "base/basic_macros.h"
#include "base/ptr/scoped_ptr.h"
#include "game/board.h"
#include "game/board_location.h"
#include "game/game.h"
#include "game/game_options.h"
#include "game/game_serializer.h"
#include "gtest/gtest.h"

namespace game {
namespace {

class GameSerializerTest : public ::testing::Test {
 public:
  virtual void SetUp() {
    // TODO(player): Remove the temporary implementation
    Reset(game_, new Game(GameOptions()));
    game()->Initialize();
    white_locations_.push_back(BoardLocation(0, 0));
    white_locations_.push_back(BoardLocation(0, 3));
    white_locations_.push_back(BoardLocation(0, 6));
    black_locations_.push_back(BoardLocation(6, 0));
    black_locations_.push_back(BoardLocation(6, 3));
    for (size_t i = 0; i < black_locations_.size(); ++i) {
      PlayerAction white_action(Board::WHITE_COLOR, PlayerAction::PLACE_PIECE);
      white_action.set_destination(white_locations_[i]);
      game()->ExecutePlayerAction(white_action);
      PlayerAction black_action(Board::BLACK_COLOR, PlayerAction::PLACE_PIECE);
      black_action.set_destination(black_locations_[i]);
      game()->ExecutePlayerAction(black_action);
    }
    PlayerAction white_action(Board::WHITE_COLOR, PlayerAction::PLACE_PIECE);
    white_action.set_destination(white_locations_[2]);
    game()->ExecutePlayerAction(white_action);
    // We have formed a mill now, so we have to remove a black piece
    PlayerAction remove_action(Board::WHITE_COLOR, PlayerAction::REMOVE_PIECE);
    remove_action.set_source(black_locations_.back());
    game()->ExecutePlayerAction(remove_action);
    black_locations_.pop_back();
  }

  Game* game() const { return Get(game_); }

 private:
  base::ptr::scoped_ptr<Game> game_;
  std::vector<BoardLocation> white_locations_;
  std::vector<BoardLocation> black_locations_;
};

TEST_F(GameSerializerTest, BinarySerialization) {
  std::ostringstream out(std::ios::out | std::ios::binary);
  GameSerializer::SerializeTo(*game(), out, true);
  const char expected_data[] = {
      // The number of moves on 64 bits
      0x06, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
      // White places at (0, 0)
      0x00, 0x01, 0x00, 0x00,
      // Black places at (6, 0)
      0x00, 0x02, 0x06, 0x00,
      // White places at (0, 3)
      0x00, 0x01, 0x00, 0x03,
      // Black places at (6, 3)
      0x00, 0x02, 0x06, 0x03,
      // White places at (0, 6)
      0x00, 0x01, 0x00, 0x06,
      // White removes from (6, 3)
      0x02, 0x01, 0x06, 0x03
  };
  const std::string binary_string = out.str();
  ASSERT_EQ(arraysize(expected_data), binary_string.size());
  const char* actual_data = binary_string.c_str();
  for (size_t i = 0; i < arraysize(expected_data); ++i) {
    EXPECT_EQ(expected_data[i], actual_data[i]);
  }
}

TEST_F(GameSerializerTest, TextSerialization) {
  std::ostringstream out;
  GameSerializer::SerializeTo(*game(), out, false);
  const std::string expected =
      "6\n"
      "PLACE WHITE 0 0\n"
      "PLACE BLACK 6 0\n"
      "PLACE WHITE 0 3\n"
      "PLACE BLACK 6 3\n"
      "PLACE WHITE 0 6\n"
      "REMOVE WHITE 6 3\n";
  EXPECT_EQ(expected, out.str());
}

TEST_F(GameSerializerTest, EmptyGame) {
  GameOptions game_options;
  Game game(game_options);

  std::ostringstream text_stream;
  GameSerializer::SerializeTo(game, text_stream, false);
  EXPECT_EQ("0\n", text_stream.str());

  std::ostringstream binary_stream(std::ios::out | std::ios::binary);
  GameSerializer::SerializeTo(game, binary_stream, true);
  const char expected[8] = { 0 };
  const std::string binary_string = binary_stream.str();
  ASSERT_EQ(arraysize(expected), binary_string.size());
  for (size_t i = 0; i < arraysize(expected); ++i) {
    EXPECT_EQ(expected[i], binary_string[i]);
  }
}

}  // anonymous namespace
}  // namespace game
