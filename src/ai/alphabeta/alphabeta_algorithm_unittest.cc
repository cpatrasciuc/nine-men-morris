// Copyright (c) 2013 Cristian Patrasciuc. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "ai/alphabeta/alphabeta_algorithm.h"
#include "ai/alphabeta/evaluators.h"
#include "game/board.h"
#include "game/game.h"
#include "game/game_options.h"
#include "game/game_type.h"
#include "gtest/gtest.h"

namespace ai {
namespace alphabeta {
namespace {

TEST(AlphaBetaAlgorithm, AlphaBetaAlgorithm) {
  game::GameOptions options;
  options.set_game_type(game::THREE_MEN_MORRIS);
  game::Game test_game(options);
  test_game.Initialize();
  AlphaBetaAlgorithm alg(options);
  game::PlayerAction action =
      static_cast<AIAlgorithm*>(&alg)->GetNextAction(test_game);
  EXPECT_FALSE(true) << action.source();
  EXPECT_FALSE(true) << action.destination();
  EXPECT_FALSE(true) << action.type();
}

}  // anonymous namespace
}  // namespace alphabeta
}  // namespace ai
