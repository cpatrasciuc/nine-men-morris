// Copyright (c) 2013 Cristian Patrasciuc. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include <cstdlib>
#include <ctime>
#include <memory>

#include "ai/ai_algorithm.h"
#include "ai/random/random_algorithm.h"
#include "base/function.h"
#include "base/log.h"
#include "base/ptr/scoped_ptr.h"
#include "game/game.h"
#include "game/game_options.h"
#include "game/piece_color.h"
#include "game/player_action.h"
#include "gtest/gtest.h"

namespace ai {
namespace random {
namespace {

TEST(RandomAlgorithm, ThreeMenMorrisFullGame) {
  // TODO(random_algorithm): Replace this random generator with a stable one
  std::srand(std::time(NULL));
  const int max_moves = 100;
  std::auto_ptr<RandomAlgorithm::RandomNumberGenerator> white_random(
      new base::Function<int(void)>(&std::rand));
  std::auto_ptr<RandomAlgorithm::RandomNumberGenerator> black_random(
      new base::Function<int(void)>(&std::rand));
  base::ptr::scoped_ptr<AIAlgorithm> white(new RandomAlgorithm(white_random));
  base::ptr::scoped_ptr<AIAlgorithm> black(new RandomAlgorithm(black_random));
  game::GameOptions game_options;
  game_options.set_game_type(game::GameOptions::THREE_MEN_MORRIS);
  game::Game test_game(game_options);
  test_game.Initialize();
  for (int i = 0; i < max_moves; ++i) {
    AIAlgorithm* next_player = Get(
        (test_game.current_player() == game::WHITE_COLOR) ? white : black);
    game::PlayerAction action(next_player->GetNextAction(test_game));
    test_game.ExecutePlayerAction(action);
    if (test_game.is_game_over()) {
      // TODO(random_algorithm): Assert that game is over once we have a stable
      // random number generator.
      LOG(INFO) << "Game is over";
      break;
    }
  }
}

}  // anonymous namespace
}  // namespace random
}  // namespace ai
