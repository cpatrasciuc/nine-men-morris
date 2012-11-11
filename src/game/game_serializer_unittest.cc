// Copyright (c) 2012 Cristian Patrasciuc. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include <stdint.h>

#include <memory>
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
  static const char kExpectedBinaryStream[];
  static const std::string kExpectedTextStream;

  virtual void SetUp() {
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

  void AssertEqualGame(const Game& actual_game) const {
    EXPECT_EQ(game_->is_game_over(), actual_game.is_game_over());
    EXPECT_EQ(game_->options().game_type(), actual_game.options().game_type());
    EXPECT_EQ(game_->options().jumps_allowed(),
              actual_game.options().jumps_allowed());
    EXPECT_EQ(game_->options().white_starts(),
              actual_game.options().white_starts());
    std::vector<PlayerAction> expected_actions;
    game_->DumpActionList(&expected_actions);
    std::vector<PlayerAction> actual_actions;
    actual_game.DumpActionList(&actual_actions);
    ASSERT_EQ(expected_actions.size(), actual_actions.size());
    for (size_t i = 0; i < expected_actions.size(); ++i) {
      EXPECT_EQ(expected_actions[i].type(), actual_actions[i].type());
      EXPECT_EQ(expected_actions[i].player_color(),
                actual_actions[i].player_color());
      EXPECT_EQ(expected_actions[i].source(), actual_actions[i].source());
      EXPECT_EQ(expected_actions[i].destination(),
                actual_actions[i].destination());
    }
  }

  Game* game() const { return Get(game_); }

 private:
  base::ptr::scoped_ptr<Game> game_;
  std::vector<BoardLocation> white_locations_;
  std::vector<BoardLocation> black_locations_;
};

const char GameSerializerTest::kExpectedBinaryStream[] = {
    // The encoding of game options
    0x32,
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

const std::string GameSerializerTest::kExpectedTextStream =
    "50\n"
    "6\n"
    "PLACE WHITE 0 0\n"
    "PLACE BLACK 6 0\n"
    "PLACE WHITE 0 3\n"
    "PLACE BLACK 6 3\n"
    "PLACE WHITE 0 6\n"
    "REMOVE WHITE 6 3\n";



TEST_F(GameSerializerTest, BinarySerialization) {
  std::ostringstream out(std::ios::out | std::ios::binary);
  GameSerializer::SerializeTo(*game(), &out, true);
  const std::string binary_string = out.str();
  ASSERT_EQ(arraysize(kExpectedBinaryStream), binary_string.size());
  const char* actual_data = binary_string.c_str();
  for (size_t i = 0; i < arraysize(kExpectedBinaryStream); ++i) {
    EXPECT_EQ(kExpectedBinaryStream[i], actual_data[i]);
  }
}

TEST_F(GameSerializerTest, BinaryDeserialization) {
  std::istringstream in(
      std::string(kExpectedBinaryStream, arraysize(kExpectedBinaryStream)),
      std::ios::in | std::ios::binary);
  std::auto_ptr<Game> game = GameSerializer::DeserializeFrom(&in, true);
  ASSERT_TRUE(game.get());
  AssertEqualGame(*game.get());
}

TEST_F(GameSerializerTest, TextSerialization) {
  std::ostringstream out;
  GameSerializer::SerializeTo(*game(), &out, false);
  EXPECT_EQ(kExpectedTextStream, out.str());
}

TEST_F(GameSerializerTest, TextDeserialization) {
  std::istringstream in(kExpectedTextStream);
  std::auto_ptr<Game> game = GameSerializer::DeserializeFrom(&in, false);
  ASSERT_TRUE(game.get());
  AssertEqualGame(*game.get());
}

TEST_F(GameSerializerTest, EmptyGame) {
  GameOptions game_options;
  Game game(game_options);

  std::ostringstream text_stream;
  GameSerializer::SerializeTo(game, &text_stream, false);
  EXPECT_EQ("50\n0\n", text_stream.str());

  std::ostringstream binary_stream(std::ios::out | std::ios::binary);
  GameSerializer::SerializeTo(game, &binary_stream, true);
  const char expected[9] = { 0x32, 0 };
  const std::string binary_string = binary_stream.str();
  ASSERT_EQ(arraysize(expected), binary_string.size());
  for (size_t i = 0; i < arraysize(expected); ++i) {
    EXPECT_EQ(expected[i], binary_string[i]);
  }
}

TEST_F(GameSerializerTest, InvalidTextStream) {
  const std::string invalid_streams[] = {
    "real invalid stream",
    "50",
    "50 invalid action number",
    "50 1 PLACE",
    "50 1 PLACE WHITE",
    "50 1 INVALID_TYPE WHITE",
    "50 1 PLACE INVALID_COLOR",
    // Invalid series of actions (the white player should move first)
    "50 1 PLACE BLACK 0 0",
  };

  for (size_t i = 0; i < arraysize(invalid_streams); ++i) {
    std::istringstream in(invalid_streams[i]);
    std::auto_ptr<Game> game = GameSerializer::DeserializeFrom(&in, false);
    EXPECT_FALSE(game.get());
  }
}

// TODO(serialization): Add tests for invalid binary streams

}  // anonymous namespace
}  // namespace game
