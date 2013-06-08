// Copyright (c) 2013 Cristian Patrasciuc. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include <vector>

#include "ai/game_state.h"
#include "ai/game_state_tree.h"
#include "base/basic_macros.h"
#include "game/board.h"
#include "game/board_location.h"
#include "game/game_options.h"
#include "game/game_type.h"
#include "gtest/gtest.h"

namespace ai {
namespace {

class GameStateTreeTest
    : public ::testing::TestWithParam<game::GameOptions> {
 public:
  GameStateTreeTest()
      : options_(GetParam()),
        board_(options_.game_type()),
        game_state_tree_(options_) {}

 protected:
  const game::GameOptions& game_options() const { return options_; }

  game::Board& board() { return board_; }

  GameStateTree& game_state_tree() { return game_state_tree_; }

  unsigned int GetTotalBoardLocationCount() {
    return board_.locations().size();
  }

 private:
  game::GameOptions options_;
  game::Board board_;
  GameStateTree game_state_tree_;
};

TEST_P(GameStateTreeTest, Place) {
  GameState start;
  start.set_pieces_in_hand(game::WHITE_COLOR, 9);
  start.set_pieces_in_hand(game::BLACK_COLOR, 9);
  start.set_current_player(game::WHITE_COLOR);
  start.Encode(board());
  std::vector<GameState> successors;
  game_state_tree().GetSuccessors(start, &successors);
  unsigned int valid_locations_count = GetTotalBoardLocationCount();
  EXPECT_EQ(valid_locations_count, successors.size());
  GameState second(successors[0]);
  EXPECT_EQ(game::BLACK_COLOR, second.current_player());
  EXPECT_EQ(8, second.pieces_in_hand(game::WHITE_COLOR));
  EXPECT_EQ(9, second.pieces_in_hand(game::BLACK_COLOR));
  successors.clear();
  game_state_tree().GetSuccessors(second, &successors);
  EXPECT_EQ(valid_locations_count - 1, successors.size());
}

TEST_P(GameStateTreeTest, PlaceAndRemoveFreePiece) {
  GameState start;
  start.set_pieces_in_hand(game::WHITE_COLOR, 9);
  start.set_pieces_in_hand(game::BLACK_COLOR, 9);
  start.set_current_player(game::WHITE_COLOR);
  const game::BoardLocation white_locations[] = {
    game::BoardLocation(0, 0),
    game::BoardLocation(board().size() / 2, 0),
    game::BoardLocation(board().size() - 1, 0)
  };
  const game::BoardLocation black_locations[] = {
    game::BoardLocation(0, board().size() - 1),
    game::BoardLocation(board().size() / 2, board().size() - 1),
    game::BoardLocation(board().size() - 1, board().size() - 1)
  };
  for (size_t i = 0; i < arraysize(white_locations); ++i) {
    EXPECT_TRUE(board().AddPiece(white_locations[i], game::WHITE_COLOR));
  }
    for (size_t i = 0; i < arraysize(black_locations); ++i) {
    EXPECT_TRUE(board().AddPiece(black_locations[i], game::BLACK_COLOR));
  }
  const game::BoardLocation free_piece(board().size() - 1, board().size() / 2);
  EXPECT_TRUE(board().AddPiece(free_piece, game::BLACK_COLOR));
  EXPECT_TRUE(board().RemovePiece(white_locations[0]));
  start.Encode(board());
  std::vector<GameState> successors;
  game_state_tree().GetSuccessors(start, &successors);
  int expected_state_count = GetTotalBoardLocationCount() -
                             arraysize(white_locations) -
                             arraysize(black_locations);
  EXPECT_EQ(expected_state_count, static_cast<int>(successors.size()));
  for (size_t i = 0; i < successors.size(); ++i) {
    game::Board decoded_board(game_options().game_type());
    successors[i].Decode(&decoded_board);
    const int decoded_black_pieces =
        decoded_board.GetPieceCountByColor(game::BLACK_COLOR);
    if (decoded_board.GetPieceAt(white_locations[0]) == game::WHITE_COLOR) {
      EXPECT_EQ(decoded_board.IsPartOfMill(game::BoardLocation(0, 0)),
                decoded_black_pieces == arraysize(black_locations));
      EXPECT_EQ(game::NO_COLOR, decoded_board.GetPieceAt(free_piece));
    } else {
      EXPECT_EQ(static_cast<int>(arraysize(black_locations)) + 1,
                decoded_black_pieces);
    }
  }
}

TEST_P(GameStateTreeTest, PlaceAndRemoveNoFreePiece) {
  GameState start;
  start.set_pieces_in_hand(game::WHITE_COLOR, 9);
  start.set_pieces_in_hand(game::BLACK_COLOR, 9);
  start.set_current_player(game::WHITE_COLOR);
  const game::BoardLocation white_locations[] = {
    game::BoardLocation(0, 0),
    game::BoardLocation(board().size() / 2, 0),
    game::BoardLocation(board().size() - 1, 0)
  };
  const game::BoardLocation black_locations[] = {
    game::BoardLocation(0, board().size() - 1),
    game::BoardLocation(board().size() / 2, board().size() - 1),
    game::BoardLocation(board().size() - 1, board().size() - 1)
  };
  for (size_t i = 0; i < arraysize(white_locations); ++i) {
    EXPECT_TRUE(board().AddPiece(white_locations[i], game::WHITE_COLOR));
  }
    for (size_t i = 0; i < arraysize(black_locations); ++i) {
    EXPECT_TRUE(board().AddPiece(black_locations[i], game::BLACK_COLOR));
  }
  EXPECT_TRUE(board().RemovePiece(white_locations[0]));
  start.Encode(board());
  std::vector<GameState> successors;
  game_state_tree().GetSuccessors(start, &successors);
  int expected_state_count = GetTotalBoardLocationCount() -
                             arraysize(white_locations) -
                             arraysize(black_locations) + 3;
  EXPECT_EQ(expected_state_count, static_cast<int>(successors.size()));
  for (size_t i = 0; i < successors.size(); ++i) {
    game::Board decoded_board(game_options().game_type());
    successors[i].Decode(&decoded_board);
    const int decoded_black_pieces =
        decoded_board.GetPieceCountByColor(game::BLACK_COLOR);
    if (decoded_board.GetPieceAt(white_locations[0]) == game::WHITE_COLOR) {
      EXPECT_EQ(decoded_board.IsPartOfMill(game::BoardLocation(0, 0)),
                decoded_black_pieces == arraysize(black_locations) - 1);
    } else {
      EXPECT_EQ(static_cast<int>(arraysize(black_locations)),
                decoded_black_pieces);
    }
  }
}

TEST_P(GameStateTreeTest, Moves) {
  GameState start;
  start.set_current_player(game::WHITE_COLOR);
  game::BoardLocation piece_location(0, 0);
  board().AddPiece(piece_location, game::WHITE_COLOR);
  start.Encode(board());
  std::vector<GameState> successors;
  game_state_tree().GetSuccessors(start, &successors);
  if (game_options().jumps_allowed()) {
    EXPECT_EQ(GetTotalBoardLocationCount() - 1, successors.size());
  } else {
    std::vector<game::BoardLocation> adjacent_locations;
    board().GetAdjacentLocations(piece_location, &adjacent_locations);
    EXPECT_EQ(adjacent_locations.size(), successors.size());
  }
}

TEST_P(GameStateTreeTest, MovesWithMill) {
  GameState state;
  state.set_current_player(game::WHITE_COLOR);
  const game::BoardLocation white_locations[] = {
    game::BoardLocation(0, board().size() / 2),
    game::BoardLocation(board().size() / 2, 0),
    game::BoardLocation(board().size() - 1, 0)
  };
  std::vector<game::BoardLocation> black_locations;
  for (size_t i = 0; i < arraysize(white_locations); ++i) {
    EXPECT_TRUE(board().AddPiece(white_locations[i], game::WHITE_COLOR));
    board().GetAdjacentLocations(white_locations[i], &black_locations);
  }
  // Place black pieces so that white can only move to (0, 0).
  int black_pieces = 0;
  for (size_t i = 0; i < black_locations.size(); ++i) {
    if (black_locations[i] == game::BoardLocation(0, 0)) {
      continue;
    }
    if (board().GetPieceAt(black_locations[i]) == game::NO_COLOR) {
      EXPECT_TRUE(board().AddPiece(black_locations[i], game::BLACK_COLOR));
      ++black_pieces;
    }
  }
  state.Encode(board());
  std::vector<GameState> successors;
  game_state_tree().GetSuccessors(state, &successors);
  int expected_state_count = black_pieces + 1;
  if (game_options().jumps_allowed()) {
    const int empty_locations = GetTotalBoardLocationCount() -
                                arraysize(white_locations) -
                                black_pieces;
    expected_state_count =
        empty_locations * arraysize(white_locations) + black_pieces - 1;
  }
  EXPECT_EQ(expected_state_count, static_cast<int>(successors.size()));
  for (size_t i = 0; i < successors.size(); ++i) {
    game::Board decoded_board(game_options().game_type());
    successors[i].Decode(&decoded_board);
    const int decoded_black_pieces =
        decoded_board.GetPieceCountByColor(game::BLACK_COLOR);
    EXPECT_EQ(decoded_board.IsPartOfMill(game::BoardLocation(0, 0)),
              decoded_black_pieces == black_pieces - 1);
  }
}

game::GameOptions ConstructGameOptions(game::GameType type, bool allow_jumps) {
  game::GameOptions result;
  result.set_game_type(type);
  result.set_jumps_allowed(allow_jumps);
  return result;
}

game::GameOptions test_scenarios[] = {
  ConstructGameOptions(game::THREE_MEN_MORRIS, true),
  ConstructGameOptions(game::THREE_MEN_MORRIS, false),
  ConstructGameOptions(game::SIX_MEN_MORRIS, true),
  ConstructGameOptions(game::SIX_MEN_MORRIS, false),
  ConstructGameOptions(game::NINE_MEN_MORRIS, true),
  ConstructGameOptions(game::NINE_MEN_MORRIS, false)
};

INSTANTIATE_TEST_CASE_P(GameStateGeneratorTestInstance,
                        GameStateTreeTest,
                        ::testing::ValuesIn(test_scenarios));

}  // anonymous namespace
}  // namespace ai

