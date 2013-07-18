// Copyright (c) 2012 Cristian Patrasciuc. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include <memory>
#include <sstream>
#include <string>
#include <vector>

#include "base/basic_macros.h"
#include "game/board.h"
#include "game/board_location.h"
#include "game/game.h"
#include "game/game_test_helper.h"
#include "game/piece_color.h"
#include "game/player_action.h"
#include "gtest/gtest.h"

namespace game {
namespace {

TEST(GameTest, ThreeMenMorris) {
  std::auto_ptr<Game> game = LoadSavedGameForTests("full_3");
  ASSERT_TRUE(game.get());
  EXPECT_TRUE(game->is_game_over());
  EXPECT_EQ(BLACK_COLOR, game->winner());
}

TEST(GameTest, SixMenMorris) {
  std::auto_ptr<Game> game = LoadSavedGameForTests("full_6");
  ASSERT_TRUE(game.get());
  EXPECT_TRUE(game->is_game_over());
  EXPECT_EQ(WHITE_COLOR, game->winner());
}

TEST(GameTest, BlackStarts) {
  GameOptions options;
  options.set_white_starts(false);
  Game game(options);
  game.Initialize();
  PlayerAction invalid_action(WHITE_COLOR, PlayerAction::PLACE_PIECE);
  invalid_action.set_destination(BoardLocation(0, 0));
  EXPECT_FALSE(game.CanExecutePlayerAction(invalid_action));
  PlayerAction valid_action(BLACK_COLOR, PlayerAction::PLACE_PIECE);
  valid_action.set_destination(BoardLocation(0, 0));
  EXPECT_TRUE(game.CanExecutePlayerAction(valid_action));
}

TEST(GameTest, ExceuteActionAfterGameIsOver) {
  std::auto_ptr<Game> game = LoadSavedGameForTests("full_3");
  ASSERT_TRUE(game.get());
  EXPECT_TRUE(game->is_game_over());
  PlayerAction action(WHITE_COLOR, PlayerAction::REMOVE_PIECE);
  EXPECT_FALSE(game->CanExecutePlayerAction(action));
}

TEST(GameTest, MoveBeforePlacingAllPieces) {
  std::auto_ptr<Game> game = LoadSavedGameForTests("place_phase_3");
  ASSERT_TRUE(game.get());
  game->UndoLastAction();
  PlayerAction move_action(BLACK_COLOR, PlayerAction::MOVE_PIECE);
  EXPECT_FALSE(game->CanExecutePlayerAction(move_action));
}

TEST(GameTest, PlaceTooManyTimes) {
  std::auto_ptr<Game> game = LoadSavedGameForTests("place_phase_3");
  ASSERT_TRUE(game.get());
  PlayerAction place_action(WHITE_COLOR, PlayerAction::PLACE_PIECE);
  place_action.set_destination(BoardLocation(1, 2));
  EXPECT_FALSE(game->CanExecutePlayerAction(place_action));
}

TEST(GameTest, UndoLastAction) {
  std::auto_ptr<Game> game = LoadSavedGameForTests("full_3");
  ASSERT_TRUE(game.get());
  EXPECT_TRUE(game->is_game_over());
  game->UndoLastAction();
  EXPECT_FALSE(game->is_game_over());
  std::vector<PlayerAction> actions;
  game->DumpActionList(&actions);
  const PlayerAction first_action(actions[0]);
  for (size_t i = 0; i < actions.size(); i++) {
    game->UndoLastAction();
  }
  actions.clear();
  game->DumpActionList(&actions);
  EXPECT_TRUE(actions.empty());
  EXPECT_TRUE(game->CanExecutePlayerAction(first_action));
  // Undoing an action for an empty game should be a no-op, not a crash
  game->UndoLastAction();
}

TEST(GameTest, RemovePieceFromOpponentMill) {
  // TODO(game_tests): This scenario might be removed since it's already tested
  // in player_action_unittest.cc
  std::auto_ptr<Game> game = LoadSavedGameForTests("remove_from_mill_6");
  ASSERT_TRUE(game.get());
  PlayerAction remove_from_mill(BLACK_COLOR, PlayerAction::REMOVE_PIECE);
  remove_from_mill.set_source(BoardLocation(4, 4));
  EXPECT_TRUE(game->board().IsPartOfMill(remove_from_mill.source()));
  EXPECT_FALSE(game->CanExecutePlayerAction(remove_from_mill));
  remove_from_mill.set_source(BoardLocation(1, 2));
  EXPECT_FALSE(game->board().IsPartOfMill(remove_from_mill.source()));
  EXPECT_TRUE(game->CanExecutePlayerAction(remove_from_mill));
}

TEST(GameTest, CanJump) {
  std::auto_ptr<Game> game = LoadSavedGameForTests("full_6");
  ASSERT_TRUE(game.get());
  std::vector<PlayerAction> actions;
  game->DumpActionList(&actions);
  for (size_t i = actions.size(); i > 0; --i) {
    game->UndoLastAction();
    if (game->next_action_type() != PlayerAction::MOVE_PIECE) {
      EXPECT_FALSE(game->CanJump()) << i;
    } else if ((i >= 27) && (game->current_player() == BLACK_COLOR)) {
      EXPECT_TRUE(game->CanJump()) << i;
    } else if ((i >= 61) && (game->current_player() == WHITE_COLOR)) {
      EXPECT_TRUE(game->CanJump()) << i;
    } else {
      EXPECT_FALSE(game->CanJump()) << i;
    }
  }

  // Now the saved game is replayed but jumps are completely disabled using the
  // game options. The first invalid move is expected to be a jump.
  GameOptions options(game->options());
  options.set_jumps_allowed(false);
  Game no_jump_game(options);
  no_jump_game.Initialize();
  for (size_t i = 0; i < actions.size(); ++i) {
    EXPECT_FALSE(no_jump_game.CanJump());
    if (no_jump_game.CanExecutePlayerAction(actions[i])) {
      no_jump_game.ExecutePlayerAction(actions[i]);
    } else {
      EXPECT_TRUE(actions[i].IsJumpOn(no_jump_game.board()));
      break;
    }
  }
}

TEST(GameDeathTest, DEBUG_ONLY_TEST(DoubleInitialization)) {
  ::testing::FLAGS_gtest_death_test_style = "threadsafe";
  Game game;
  game.Initialize();
  ASSERT_DEATH(game.Initialize(), "");
}

}  // anonymous namespace
}  // namespace game

