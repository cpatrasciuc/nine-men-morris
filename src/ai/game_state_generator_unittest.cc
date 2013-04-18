// Copyright (c) 2013 Cristian Patrasciuc. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include <vector>

#include "ai/game_state.h"
#include "ai/game_state_generator.h"
#include "base/basic_macros.h"
#include "game/board.h"
#include "game/board_location.h"
// TODO(game_type): Extract the GameType enum in a separate header.
#include "game/game_options.h"
#include "gtest/gtest.h"

namespace ai {
namespace {

class GameStateGeneratorTest
    : public ::testing::TestWithParam<game::GameOptions> {
 public:
  GameStateGeneratorTest()
      : options_(GetParam()),
        board_(options_.game_type()),
        generator_(options_) {}

 protected:
  const game::GameOptions& game_options() const { return options_; }

  game::Board& board() { return board_; }

  GameStateGenerator& generator() { return generator_; }

  unsigned int GetTotalBoardLocationCount() {
    unsigned int valid_locations_count = 0U;
    for (int line = 0; line < board().size(); ++line) {
      for (int col = 0; col < board().size(); ++col) {
        if (board().IsValidLocation(game::BoardLocation(line, col))) {
          ++valid_locations_count;
        }
      }
    }
    return valid_locations_count;
  }

 private:
  game::GameOptions options_;
  game::Board board_;
  GameStateGenerator generator_;
};

TEST_P(GameStateGeneratorTest, Place) {
  GameState start;
  start.set_pieces_in_hand(game::WHITE_COLOR, 9);
  start.set_pieces_in_hand(game::BLACK_COLOR, 9);
  start.set_current_player(game::WHITE_COLOR);
  start.Encode(board());
  std::vector<GameState> successors;
  generator().GetSuccessors(start, &successors);
  unsigned int valid_locations_count = GetTotalBoardLocationCount();
  EXPECT_EQ(valid_locations_count, successors.size());
  GameState second(successors[0]);
  EXPECT_EQ(game::BLACK_COLOR, second.current_player());
  EXPECT_EQ(8, second.pieces_in_hand(game::WHITE_COLOR));
  EXPECT_EQ(9, second.pieces_in_hand(game::BLACK_COLOR));
  successors.clear();
  generator().GetSuccessors(second, &successors);
  EXPECT_EQ(valid_locations_count - 1, successors.size());
}

TEST_P(GameStateGeneratorTest, Moves) {
  GameState start;
  start.set_current_player(game::WHITE_COLOR);
  game::BoardLocation piece_location(0, 0);
  board().AddPiece(piece_location, game::WHITE_COLOR);
  start.Encode(board());
  std::vector<GameState> successors;
  generator().GetSuccessors(start, &successors);
  // TODO(game_state): Write unit tests for moves that close a mill
  if (game_options().jumps_allowed()) {
    EXPECT_EQ(GetTotalBoardLocationCount() - 1, successors.size());
  } else {
    std::vector<game::BoardLocation> adjacent_locations;
    board().GetAdjacentLocations(piece_location, &adjacent_locations);
    EXPECT_EQ(adjacent_locations.size(), successors.size());
  }
}

game::GameOptions ConstructGameOptions(game::GameOptions::GameType type,
                                       bool allow_jumps) {
  game::GameOptions result;
  result.set_game_type(type);
  result.set_jumps_allowed(allow_jumps);
  return result;
}

game::GameOptions test_scenarios[] = {
  ConstructGameOptions(game::GameOptions::THREE_MEN_MORRIS, true),
  ConstructGameOptions(game::GameOptions::THREE_MEN_MORRIS, false),
  ConstructGameOptions(game::GameOptions::SIX_MEN_MORRIS, true),
  ConstructGameOptions(game::GameOptions::SIX_MEN_MORRIS, false),
  ConstructGameOptions(game::GameOptions::NINE_MEN_MORRIS, true),
  ConstructGameOptions(game::GameOptions::NINE_MEN_MORRIS, false)
};

INSTANTIATE_TEST_CASE_P(GameStateGeneratorTestInstance,
                        GameStateGeneratorTest,
                        ::testing::ValuesIn(test_scenarios));

}  // anonymous namespace
}  // namespace ai

