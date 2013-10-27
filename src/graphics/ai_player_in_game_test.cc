// Copyright (c) 2013 Cristian Patrasciuc. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include <memory>

#include "game/game.h"
#include "game/game_listener.h"
#include "game/game_options.h"
#include "game/game_type.h"
#include "game/piece_color.h"
#include "game/player_action.h"
#include "graphics/ai_player.h"
#include "graphics/in_game_test.h"
#include "graphics/player_delegate.h"
#include "graphics/playing_state.h"

namespace graphics {
namespace {

// Run a three men morris game with two AI players for 10 moves, then quit.
class AIPlayerInGameTest : public InGameTestBase, public game::GameListener {
 public:
  virtual void TestMethod() {
    actions_count_ = 0;
    std::auto_ptr<PlayerDelegate> white_player(new AIPlayer());
    std::auto_ptr<PlayerDelegate> black_player(new AIPlayer());
    game::GameOptions options;
    options.set_game_type(game::THREE_MEN_MORRIS);
    std::auto_ptr<game::Game> game_model(new game::Game(options));
    game_model->Initialize();
    game_model->AddListener(this);
    Reset(playing_state_,
          new PlayingState(game_model, white_player, black_player));
    app()->PushState(Get(playing_state_));
  }

  virtual void OnPlayerAction(const game::PlayerAction& action) {
    ++actions_count_;
    if (actions_count_ > 10) {
      SUCCEED();
    }
  }

  virtual void OnGameOver(game::PieceColor winner) {
    FAIL() << "AI vs. AI should be a draw. "
           << "It should not end in less than 10 moves.";
  }

  virtual void Done() {
    ASSERT_EQ(Get(playing_state_), app()->PopState());
    Reset(playing_state_);
    InGameTestBase::Done();
  }

 private:
  base::ptr::scoped_ptr<GameState> playing_state_;
  int actions_count_;
};

IN_GAME_TEST(AIPlayerInGameTest, AIvsAI);

}  // anonymous namespace
}  // namespace graphics
