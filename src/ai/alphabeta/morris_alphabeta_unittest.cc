// Copyright (c) 2013 Cristian Patrasciuc. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "ai/alphabeta/morris_alphabeta.h"
#include "ai/alphabeta/evaluators.h"
#include "ai/random/random_algorithm.h"
#include "base/function.h"
#include "base/ptr/scoped_ptr.h"
#include "base/random.h"
#include "game/board.h"
#include "game/board_location.h"
#include "game/game.h"
#include "game/game_options.h"
#include "game/game_type.h"
#include "gtest/gtest.h"

namespace ai {
namespace alphabeta {
namespace {

// Evaluator that favors pieces in the upper corner of the board.
int TestEvaluator(const game::Board& board, game::PieceColor player) {
  const game::BoardLocation loc(board.size() - 1, board.size() - 1);
  const game::PieceColor color = board.GetPieceAt(loc);
  if (color == player) {
    return 100;
  }
  if (color == game::NO_COLOR) {
    return 0;
  }
  return -100;
}

TEST(MorrisAlphaBeta, Evaluators) {
  std::vector<Evaluator*> evaluators;
  evaluators.push_back(new base::Function<EvaluatorSignature>(&TestEvaluator));
  game::GameOptions options;
  options.set_game_type(game::THREE_MEN_MORRIS);
  game::Game test_game(options);
  test_game.Initialize();
  MorrisAlphaBeta alg(options, evaluators);
  alg.set_max_search_depth(2);
  game::PlayerAction action =
      static_cast<AIAlgorithm*>(&alg)->GetNextAction(test_game);
  const int board_size = test_game.board().size();
  const game::BoardLocation upper_corner(board_size - 1, board_size - 1);
  EXPECT_EQ(game::PlayerAction::PLACE_PIECE, action.type());
  EXPECT_EQ(upper_corner, action.destination());
}

void RunTestGame(game::GameType game_type, bool jumps_allowed) {
  const int max_moves = 250;
  game::GameOptions options;
  options.set_game_type(game_type);
  options.set_jumps_allowed(jumps_allowed);
  game::Game test_game(options);
  base::ptr::scoped_ptr<AIAlgorithm> white(new random::RandomAlgorithm());
  MorrisAlphaBeta* morris_alphabeta = new MorrisAlphaBeta(options);
  morris_alphabeta->set_max_search_depth(10);
  morris_alphabeta->set_max_search_time(500000000);
  base::ptr::scoped_ptr<AIAlgorithm> black(morris_alphabeta);
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

class MorrisAlphaBetaFullGameTest
    : public ::testing::TestWithParam<game::GameType> {
};

TEST_P(MorrisAlphaBetaFullGameTest, NoJumpsAllowed) {
  RunTestGame(GetParam(), false);
}

// TODO(alphabeta): Fix flaky test.
TEST_P(MorrisAlphaBetaFullGameTest, DISABLED_JumpsAllowed) {
  RunTestGame(GetParam(), true);
}

INSTANTIATE_TEST_CASE_P(MorrisAlphaBetaFullGameTest,
                        MorrisAlphaBetaFullGameTest,
                        ::testing::Values(game::THREE_MEN_MORRIS,
                                          game::SIX_MEN_MORRIS,
                                          game::NINE_MEN_MORRIS));

}  // anonymous namespace
}  // namespace alphabeta
}  // namespace ai
