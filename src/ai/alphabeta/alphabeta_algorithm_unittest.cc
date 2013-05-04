// Copyright (c) 2013 Cristian Patrasciuc. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "ai/alphabeta/alphabeta_algorithm.h"
#include "base/function.h"
#include "game/board.h"
#include "game/game.h"
#include "game/game_options.h"
#include "game/game_type.h"
#include "gtest/gtest.h"

namespace ai {
namespace alphabeta {
namespace {

double Eval(const game::Board& board, bool max_player) {
  return max_player ? 1 : -1;
}

TEST(AB, AB) {
  std::vector<Evaluator> evaluators;
  evaluators.push_back(
      new base::Function<double(const game::Board&, bool)>(&Eval));
  game::GameOptions options;
  options.set_game_type(game::THREE_MEN_MORRIS);
  game::Game test_game(options);
  test_game.Initialize();
  AlphaBetaAlgorithm alg(options, 10, evaluators);
  game::PlayerAction action =
      static_cast<AIAlgorithm*>(&alg)->GetNextAction(test_game);
  EXPECT_FALSE(true) << action.source();
  EXPECT_FALSE(true) << action.destination();
  EXPECT_FALSE(true) << action.type();
}

}  // anonymous namespace
}  // namespace alphabeta
}  // namespace ai
