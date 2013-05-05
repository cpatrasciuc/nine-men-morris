// Copyright (c) 2013 Cristian Patrasciuc. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef AI_ALPHABETA_ALPHABETA_ALGORITHM_H_
#define AI_ALPHABETA_ALPHABETA_ALGORITHM_H_

#include <map>
#include <vector>

#include "ai/ai_algorithm.h"
#include "ai/ai_export.h"
#include "ai/alphabeta/alphabeta.h"
#include "ai/alphabeta/evaluators.h"
#include "ai/game_state.h"
#include "ai/game_state_generator.h"
#include "base/basic_macros.h"
#include "game/board_location.h"
#include "game/piece_color.h"
#include "game/player_action.h"

namespace game {
class Game;
class GameOptions;
}

namespace ai {
namespace alphabeta {

class AI_EXPORT AlphaBetaAlgorithm
    : public AIAlgorithm, public AlphaBeta<GameState>::Delegate {
 public:
  explicit AlphaBetaAlgorithm(const game::GameOptions& options);
  AlphaBetaAlgorithm(const game::GameOptions& options,
      int search_depth,
      const std::vector<Evaluator>& evaluators,
      const std::vector<int>& weights = std::vector<int>());
  ~AlphaBetaAlgorithm();

 private:
  // AIAlgorithm interface
  virtual game::PlayerAction GetNextAction(const game::Game& game_model);

  // AlphaBeta<GameState, double>::Delegate interface
  virtual bool IsTerminal(const GameState& state);
  virtual int Evaluate(const GameState& state, bool max_player);
  virtual void GetSuccessors(const GameState& state,
                             std::vector<GameState>* successors);

  const game::GameOptions& options_;

  int depth_;

  std::vector<Evaluator> evaluators_;
  std::vector<int> weights_;

  GameStateGenerator generator_;

  game::BoardLocation remove_location_;

  typedef std::map<GameState, std::vector<GameState> > SuccessorCache;
  SuccessorCache successors_cache_;

  typedef std::map<GameState, int> ScoreCache;
  ScoreCache score_cache_;

  game::PieceColor max_player_color_;

  DISALLOW_COPY_AND_ASSIGN(AlphaBetaAlgorithm);
};

}  // namespace alphabeta
}  // namespace ai

#endif  // AI_ALPHABETA_ALPHABETA_ALGORITHM_H_

