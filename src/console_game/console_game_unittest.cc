// Copyright (c) 2013 Cristian Patrasciuc. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include <memory>
#include <queue>
#include <string>
#include <vector>

#include "base/basic_macros.h"
#include "console_game/console_game.h"
#include "console_game/player.h"
#include "game/game.h"
#include "game/game_options.h"
#include "game/game_test_helper.h"
#include "game/player_action.h"
#include "game/piece_color.h"
#include "gtest/gtest.h"

namespace console_game {
namespace {

class TestPlayer : public Player {
 public:
  TestPlayer() : Player("Test Player"), action_queue_() {}

  void AddActionToQueue(const game::PlayerAction& action) {
    action_queue_.push(action);
  }

 private:
  // Player interface
  virtual std::string GetNextAction(game::Game* game_model) {
    EXPECT_FALSE(action_queue_.empty());
    game_model->ExecutePlayerAction(action_queue_.front());
    action_queue_.pop();
    return std::string();
  }

  std::queue<game::PlayerAction> action_queue_;

  DISALLOW_COPY_AND_ASSIGN(TestPlayer);
};

TEST(ConsoleGame, FullSixMenMorrisGame) {
  std::auto_ptr<game::Game> test_game(game::LoadSavedGameForTests("full_6"));
  ASSERT_TRUE(test_game.get());
  std::vector<game::PlayerAction> actions;
  test_game->DumpActionList(&actions);
  TestPlayer* white_player(new TestPlayer);
  TestPlayer* black_player(new TestPlayer);
  for (size_t i = 0; i < actions.size(); ++i) {
    if (actions[i].player_color() == game::WHITE_COLOR) {
      white_player->AddActionToQueue(actions[i]);
    } else {
      black_player->AddActionToQueue(actions[i]);
    }
  }
  game::GameOptions options;
  options.set_game_type(game::SIX_MEN_MORRIS);
  ConsoleGame test_console_game(options,
                                std::auto_ptr<Player>(white_player),
                                std::auto_ptr<Player>(black_player));
  test_console_game.Run();
}

}  // anonymous namespace
}  // namespace console_game
