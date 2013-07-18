// Copyright (c) 2012 Cristian Patrasciuc. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include <stdint.h>

#include <memory>
#include <sstream>
#include <string>
#include <vector>

#include "base/basic_macros.h"
#include "base/string_util.h"
#include "base/ptr/scoped_ptr.h"
#include "game/board.h"
#include "game/board_location.h"
#include "game/game.h"
#include "game/game_options.h"
#include "game/game_type.h"
#include "game/game_serializer.h"
#include "gtest/gtest.h"

namespace game {
namespace {

class GameSerializerTest : public ::testing::Test {
 public:
  static const char kExpectedBinaryStream[];
  static const std::string kExpectedTextStream;

  virtual void SetUp() {
    GameOptions options;
    options.set_game_type(THREE_MEN_MORRIS);
    Reset(game_, new Game(options));
    game()->Initialize();
    std::vector<BoardLocation> white_locations;
    std::vector<BoardLocation> black_locations;
    white_locations.push_back(BoardLocation(0, 0));
    white_locations.push_back(BoardLocation(0, 1));
    white_locations.push_back(BoardLocation(1, 2));
    black_locations.push_back(BoardLocation(2, 0));
    black_locations.push_back(BoardLocation(2, 1));
    black_locations.push_back(BoardLocation(1, 0));
    for (size_t i = 0; i < black_locations.size(); ++i) {
      PlayerAction white_action(WHITE_COLOR, PlayerAction::PLACE_PIECE);
      white_action.set_destination(white_locations[i]);
      game()->ExecutePlayerAction(white_action);
      PlayerAction black_action(BLACK_COLOR, PlayerAction::PLACE_PIECE);
      black_action.set_destination(black_locations[i]);
      game()->ExecutePlayerAction(black_action);
    }
    PlayerAction white_action(WHITE_COLOR, PlayerAction::MOVE_PIECE);
    white_action.set_source(BoardLocation(1, 2));
    white_action.set_destination(BoardLocation(0, 2));
    game()->ExecutePlayerAction(white_action);
    // We have formed a mill now, so we have to remove a black piece
    PlayerAction remove_action(WHITE_COLOR, PlayerAction::REMOVE_PIECE);
    remove_action.set_source(black_locations.back());
    game()->ExecutePlayerAction(remove_action);
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
};

const char GameSerializerTest::kExpectedBinaryStream[] = {
    // The serialization format version
    0x00, 0x01, 0x00, 0x00,
    // The encoding of game options
    0b00110000,
    // The number of moves on 64 bits
    0x08, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    // White places at (0, 0)
    0x00, 0x01, 0x00, 0x00,
    // Black places at (2, 0)
    0x00, 0x02, 0x02, 0x00,
    // White places at (0, 1)
    0x00, 0x01, 0x00, 0x01,
    // Black places at (2, 1)
    0x00, 0x02, 0x02, 0x01,
    // White places at (1, 2)
    0x00, 0x01, 0x01, 0x02,
    // Black places at (1, 0)
    0x00, 0x02, 0x01, 0x00,
    // White moves from (1, 2) to (0, 2)
    0x01, 0x01, 0x01, 0x02, 0x00, 0x02,
    // White removes from (1, 0)
    0x02, 0x01, 0x01, 0x00
};

const std::string GameSerializerTest::kExpectedTextStream =
    "256\n"
    "48\n"
    "8\n"
    "PLACE WHITE 0 0\n"
    "PLACE BLACK 2 0\n"
    "PLACE WHITE 0 1\n"
    "PLACE BLACK 2 1\n"
    "PLACE WHITE 1 2\n"
    "PLACE BLACK 1 0\n"
    "MOVE WHITE 1 2 0 2\n"
    "REMOVE WHITE 1 0\n";

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
  const Game game;

  std::ostringstream text_stream;
  GameSerializer::SerializeTo(game, &text_stream, false);
  EXPECT_EQ("256\n50\n0\n", text_stream.str());

  std::ostringstream binary_stream(std::ios::out | std::ios::binary);
  GameSerializer::SerializeTo(game, &binary_stream, true);
  const char expected[13] = { 0x00, 0x01, 0x00, 0x00, 0x32, 0 };
  const std::string binary_string = binary_stream.str();
  ASSERT_EQ(arraysize(expected), binary_string.size());
  for (size_t i = 0; i < arraysize(expected); ++i) {
    EXPECT_EQ(expected[i], binary_string[i]);
  }
}

TEST_F(GameSerializerTest, InvalidTextStream) {
  const std::string invalid_streams[] = {
    "",
    "real invalid stream",
    "2",
    base::ToString(GameSerializer::Version()),
    "256 50",
    "256 50 invalid action number",
    "256 50 1",
    "256 50 1 PLACE",
    "256 50 1 PLACE WHITE",
    "256 50 1 INVALID_TYPE WHITE",
    "256 50 1 PLACE INVALID_COLOR",
    // Invalid series of actions (the white player should move first)
    "256 50 1 PLACE BLACK 0 0",
  };

  for (size_t i = 0; i < arraysize(invalid_streams); ++i) {
    std::istringstream in(invalid_streams[i]);
    std::auto_ptr<Game> game = GameSerializer::DeserializeFrom(&in, false);
    EXPECT_FALSE(game.get());
  }
}

TEST_F(GameSerializerTest, InvalidBinaryStream) {
  // On the first position of each array, we store its length. The stream's
  // actual data starts only from index 1.
  const char invalid_streams[][50] = {
    // Empty stream
    { 0, 0 },
    // Invalid version
    { 1, 0 },
    // No action count
    { 5, 0x00, 0x01, 0x00, 0x00, 0x32 },
    // Not enough bits for action count
    { 6, 0x00, 0x01, 0x00, 0x00, 0x32, 0x10 },
    // Not enough actions
    { 13,
      0x00, 0x01, 0x00, 0x00,
      0x32,
      0x10, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 },
    // Incomplete action
    { 14,
      0x00, 0x01, 0x00, 0x00,
      0x32,
      0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
      0x00 },
    { 15,
      0x00, 0x01, 0x00, 0x00,
      0x32,
      0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
      0x00, 0x01 },
    { 16,
      0x00, 0x01, 0x00, 0x00,
      0x32,
      0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
      0x00, 0x01, 0x00 },
    // Invalid player color
    { 17,
      0x00, 0x01, 0x00, 0x00,
      0x32,
      0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
      0x00, static_cast<char>(0xFF), 0x00, 0x00 },
    // Invalid action type
    { 17,
      0x00, 0x01, 0x00, 0x00,
      0x32,
      0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
      static_cast<char>(0xFF), 0x01, 0x00, 0x00 },
    // Invalid series of actions (white should move first)
    { 17,
      0x00, 0x01, 0x00, 0x00,
      0x32,
      0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
      0x00, 0x02, 0x00, 0x00 },
  };

  for (size_t i = 0; i < arraysize(invalid_streams); ++i) {
    std::istringstream in(
            std::string(&invalid_streams[i][1], invalid_streams[i][0]),
            std::ios::in | std::ios::binary);
    std::auto_ptr<Game> game = GameSerializer::DeserializeFrom(&in, true);
    ASSERT_FALSE(game.get()) << i;
  }
}

}  // anonymous namespace
}  // namespace game
