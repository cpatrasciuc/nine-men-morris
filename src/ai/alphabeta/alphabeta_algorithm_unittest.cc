// Copyright (c) 2013 Cristian Patrasciuc. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "ai/alphabeta/alphabeta_algorithm.h"
#include "ai/alphabeta/evaluators.h"
#include "ai/random/random_algorithm.h"
#include "base/function.h"
#include "base/ptr/scoped_ptr.h"
#include "base/random.h"
#include "game/board.h"
#include "game/board_location.h"
#include "game/game.h"
#include "game/game_options.h"
#include "game/game_serializer.h"
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

TEST(AlphaBetaAlgorithm, Evaluators) {
  std::vector<Evaluator> evaluators;
  evaluators.push_back(new base::Function<EvaluatorSignature>(&TestEvaluator));
  game::GameOptions options;
  options.set_game_type(game::THREE_MEN_MORRIS);
  game::Game test_game(options);
  test_game.Initialize();
  AlphaBetaAlgorithm alg(options, 2, evaluators);
  game::PlayerAction action =
      static_cast<AIAlgorithm*>(&alg)->GetNextAction(test_game);
  const int board_size = test_game.board().size();
  const game::BoardLocation upper_corner(board_size - 1, board_size - 1);
  EXPECT_EQ(game::PlayerAction::PLACE_PIECE, action.type());
  EXPECT_EQ(upper_corner, action.destination());
}

TEST(AlphaBetaAlgorithm, VsRandom) {
  const int max_moves = 250;
  game::GameOptions options;
  options.set_game_type(game::THREE_MEN_MORRIS);
  options.set_jumps_allowed(false);
  game::Game test_game(options);
  base::ptr::scoped_ptr<AIAlgorithm> white(new random::RandomAlgorithm());
  base::ptr::scoped_ptr<AIAlgorithm> black(new AlphaBetaAlgorithm(options));
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
  if (test_game.winner() != game::BLACK_COLOR) {
    game::GameSerializer::SerializeTo(test_game,
        base::Log::default_output_stream, false);
    FAIL() << "AlphaBeta could not beat Random";
  }
}

}  // anonymous namespace
}  // namespace alphabeta
}  // namespace ai
