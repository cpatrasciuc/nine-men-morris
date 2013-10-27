// Copyright (c) 2013 Cristian Patrasciuc. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include <memory>
#include <vector>

#include "base/basic_macros.h"
#include "game/game.h"
#include "game/game_listener.h"
#include "game/game_test_helper.h"
#include "game/piece_color.h"
#include "game/player_action.h"
#include "gtest/gtest.h"

namespace game {
namespace {

class TestListener : public GameListener {
 public:
  TestListener() : initialized_(0), actions_played_(0), actions_undone_(0),
    winner_(NO_COLOR) {}

  virtual void OnGameInitialized() {
    ++initialized_;
  }

  virtual void OnPlayerAction(const PlayerAction& action) {
    ++actions_played_;
  }

  virtual void OnUndoPlayerAction(const PlayerAction& action) {
    ++actions_undone_;
  }

  virtual void OnGameOver(PieceColor winner) {
    winner_ = winner;
  }

  int initialized() const { return initialized_; }
  int actions_played() const { return actions_played_; }
  int actions_undone() const { return actions_undone_; }
  PieceColor winner() const { return winner_; }

 private:
  int initialized_;
  int actions_played_;
  int actions_undone_;
  PieceColor winner_;

  DISALLOW_COPY_AND_ASSIGN(TestListener);
};

TEST(GameListener, Basic) {
  std::auto_ptr<Game> test_game = LoadSavedGameForTests("full_6");
  Game monitored_game(test_game->options());
  TestListener listener;
  monitored_game.AddListener(&listener);
  monitored_game.Initialize();
  std::vector<PlayerAction> actions;
  test_game->DumpActionList(&actions);
  for (size_t i = 0; i < actions.size(); ++i) {
    monitored_game.ExecutePlayerAction(actions[i]);
  }
  EXPECT_EQ(1, listener.initialized());
  EXPECT_EQ(static_cast<int>(actions.size()), listener.actions_played());
  EXPECT_EQ(0, listener.actions_undone());
  EXPECT_EQ(test_game->winner(), listener.winner());
  for (size_t i = 0; i < actions.size(); ++i) {
    monitored_game.UndoLastAction();
  }
  EXPECT_EQ(1, listener.initialized());
  EXPECT_EQ(static_cast<int>(actions.size()), listener.actions_played());
  EXPECT_EQ(static_cast<int>(actions.size()), listener.actions_undone());
  EXPECT_EQ(test_game->winner(), listener.winner());
  monitored_game.RemoveListener(&listener);
}

}  // namespace
}  // namespace game
