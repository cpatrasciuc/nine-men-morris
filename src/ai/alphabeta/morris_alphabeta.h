// Copyright (c) 2013 Cristian Patrasciuc. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef AI_ALPHABETA_MORRIS_ALPHABETA_H_
#define AI_ALPHABETA_MORRIS_ALPHABETA_H_

#include <vector>

#include "ai/ai_algorithm.h"
#include "ai/ai_export.h"
#include "ai/alphabeta/alphabeta.h"
#include "ai/alphabeta/evaluators.h"
#include "ai/game_state.h"
#include "ai/game_state_tree.h"
#include "base/basic_macros.h"
#include "base/hash_map.h"
#include "game/board_location.h"
#include "game/piece_color.h"
#include "game/player_action.h"

namespace game {
class Game;
class GameOptions;
}

namespace ai {
namespace alphabeta {

// Specialize the hashing function for GameState objects.
template <> size_t Hash<GameState>(const GameState& state);

class AI_EXPORT MorrisAlphaBeta
    : public AIAlgorithm, public AlphaBeta<GameState>::Delegate {
 public:
  explicit MorrisAlphaBeta(const game::GameOptions& options);
  MorrisAlphaBeta(const game::GameOptions& options,
      const std::vector<Evaluator*>& evaluators,
      const std::vector<int>& weights = std::vector<int>());
  ~MorrisAlphaBeta();

  // See the similar methods from the generic AlphaBeta algorithm for details.
  int max_search_depth() const { return max_search_depth_; }
  void set_max_search_depth(int max_depth) { max_search_depth_ = max_depth; }
  int max_search_time() const { return max_search_time_; }
  void set_max_search_time(int max_time) { max_search_time_ = max_time; }

 private:
  // AIAlgorithm interface
  virtual game::PlayerAction GetNextAction(const game::Game& game_model);

  // AlphaBeta<GameState, double>::Delegate interface
  virtual bool IsTerminal(const GameState& state);
  virtual int Evaluate(const GameState& state);
  virtual void GetSuccessors(const GameState& state,
                             std::vector<GameState>* successors);

  const game::GameOptions& options_;

  int max_search_depth_;
  int max_search_time_;

  std::vector<Evaluator*> evaluators_;
  std::vector<int> weights_;

  GameStateTree tree_;

  game::BoardLocation remove_location_;

  typedef base::hash_map<GameState, int, GameStateHasher> ScoreCache;  // NOLINT
  ScoreCache score_cache_;

  game::PieceColor max_player_color_;

  DISALLOW_COPY_AND_ASSIGN(MorrisAlphaBeta);
};

}  // namespace alphabeta
}  // namespace ai

#endif  // AI_ALPHABETA_MORRIS_ALPHABETA_H_

