// Copyright (c) 2013 Cristian Patrasciuc. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include <cstdlib>
#include <ctime>
#include <memory>

#include "ai/ai_algorithm.h"
#include "ai/random/random_algorithm.h"
#include "base/bind.h"
#include "base/function.h"
#include "base/log.h"
#include "base/ptr/scoped_ptr.h"
#include "base/random.h"
#include "game/game.h"
#include "game/game_options.h"
#include "game/piece_color.h"
#include "game/player_action.h"
#include "gtest/gtest.h"

namespace ai {
namespace random {
namespace {

int GetNextRandomAsInt(base::MersenneTwister32* generator) {
  return static_cast<int>(generator->Next());
}

RandomAlgorithm* GetRandomTestPlayer(base::MersenneTwister32* generator) {
  base::Function<int(base::MersenneTwister32*)>* wrapper =
      new base::Function<int(base::MersenneTwister32*)>(&GetNextRandomAsInt);
  std::auto_ptr<RandomAlgorithm::RandomNumberGenerator> bind_ptr(
      base::Bind(wrapper, generator));
  return new RandomAlgorithm(bind_ptr);
}

TEST(RandomAlgorithm, ThreeMenMorrisFullGame) {
  const int max_moves = 100;
  base::MersenneTwister32 white_random(12345);
  base::MersenneTwister32 black_random(54321);
  base::ptr::scoped_ptr<AIAlgorithm> white(GetRandomTestPlayer(&white_random));
  base::ptr::scoped_ptr<AIAlgorithm> black(GetRandomTestPlayer(&black_random));
  game::GameOptions game_options;
  game_options.set_game_type(game::GameOptions::THREE_MEN_MORRIS);
  game::Game test_game(game_options);
  test_game.Initialize();
  for (int i = 0; i < max_moves; ++i) {
    AIAlgorithm* next_player =
        Get((test_game.current_player() == game::WHITE_COLOR) ? white : black);
    game::PlayerAction action(next_player->GetNextAction(test_game));
    test_game.ExecutePlayerAction(action);
    if (test_game.is_game_over()) {
      break;
    }
  }
  EXPECT_TRUE(test_game.is_game_over());
}

}  // anonymous namespace
}  // namespace random
}  // namespace ai
