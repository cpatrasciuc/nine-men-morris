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

using base::MersenneTwister32;
using base::ptr::scoped_ptr;

int GetNextRandomAsInt(MersenneTwister32* generator) {
  return static_cast<int>(generator->Next());
}

RandomAlgorithm* GetRandomTestPlayer(MersenneTwister32* generator) {
  typedef base::Function<int(MersenneTwister32*)> RandomWrapper;
  typedef RandomAlgorithm::RandomNumberGenerator RandomNumberGenerator;
  RandomWrapper* wrapper = new RandomWrapper(&GetNextRandomAsInt);
  std::auto_ptr<RandomNumberGenerator> binding(base::Bind(wrapper, generator));
  return new RandomAlgorithm(binding);
}

void RunTestGame(game::GameOptions::GameType game_type, bool allow_jumps) {
  const int max_moves = 250;
  MersenneTwister32 white_random(12345);
  MersenneTwister32 black_random(54321);
  scoped_ptr<AIAlgorithm> white(GetRandomTestPlayer(&white_random));
  scoped_ptr<AIAlgorithm> black(GetRandomTestPlayer(&black_random));
  game::GameOptions game_options;
  game_options.set_game_type(game_type);
  game_options.set_jumps_allowed(allow_jumps);
  game::Game test_game(game_options);
  test_game.Initialize();
  for (int i = 0; i < max_moves; ++i) {
    AIAlgorithm* next_player =
        Get((test_game.current_player() == game::WHITE_COLOR) ? white : black);
    game::PlayerAction action(next_player->GetNextAction(test_game));
    test_game.ExecutePlayerAction(action);
    if (test_game.is_game_over()) {
      LOG(INFO) << "Finished after " << i << " moves.";
      break;
    }
  }
  EXPECT_TRUE(test_game.is_game_over());
}

class RandomAlgorithmFullGameTest
    : public ::testing::TestWithParam<game::GameOptions::GameType> {
};

TEST_P(RandomAlgorithmFullGameTest, NoJumpsAllowed) {
  RunTestGame(GetParam(), false);
}

TEST_P(RandomAlgorithmFullGameTest, JumpsAllowed) {
  RunTestGame(GetParam(), true);
}

const game::GameOptions::GameType test_game_types[] = {
    game::GameOptions::THREE_MEN_MORRIS,
    game::GameOptions::SIX_MEN_MORRIS,
    game::GameOptions::NINE_MEN_MORRIS
};
INSTANTIATE_TEST_CASE_P(RandomAlgorithmTest,
                        RandomAlgorithmFullGameTest,
                        ::testing::ValuesIn(test_game_types));

}  // anonymous namespace
}  // namespace random
}  // namespace ai
