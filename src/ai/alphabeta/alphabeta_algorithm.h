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
#include "ai/game_state.h"
#include "ai/game_state_generator.h"
#include "base/basic_macros.h"
#include "base/callable.h"
#include "game/board_location.h"
#include "game/player_action.h"

namespace game {
class Game;
class GameOptions;
}

namespace ai {
namespace alphabeta {

typedef base::Callable<double(const game::Board&, bool)>* Evaluator;

class AI_EXPORT AlphaBetaAlgorithm
    : public AIAlgorithm, public AlphaBeta<GameState, double>::Delegate {
 public:
  AlphaBetaAlgorithm(const game::GameOptions& options,
      int search_depth,
      const std::vector<Evaluator>& evaluators,
      const std::vector<double>& weights = std::vector<double>());
  ~AlphaBetaAlgorithm();

 private:
  // AIAlgorithm interface
  virtual game::PlayerAction GetNextAction(const game::Game& game_model);

  // AlphaBeta<GameState, double>::Delegate interface
  virtual bool IsTerminal(const GameState& state);
  virtual double Evaluate(const GameState& state, bool max_player);
  virtual void GetSuccessors(const GameState& state,
                             std::vector<GameState>* successors);

  const game::GameOptions& options_;

  int depth_;

  std::vector<Evaluator> evaluators_;
  std::vector<double> weights_;

  GameStateGenerator generator_;

  game::BoardLocation remove_location_;

  typedef std::map<GameState, std::vector<GameState> > SuccessorCache;
  SuccessorCache successors_cache_;

  DISALLOW_COPY_AND_ASSIGN(AlphaBetaAlgorithm);
};

}  // namespace alphabeta
}  // namespace ai

#endif  // AI_ALPHABETA_ALPHABETA_ALGORITHM_H_

