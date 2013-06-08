// Copyright (c) 2013 Cristian Patrasciuc. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef AI_GAME_STATE_TREE_H_
#define AI_GAME_STATE_TREE_H_

#include <vector>

#include "ai/ai_export.h"
#include "ai/game_state.h"
#include "base/basic_macros.h"
#include "base/hash_map.h"
#include "game/game_options.h"

namespace ai {

class AI_EXPORT GameStateTree {
 public:
  explicit GameStateTree(const game::GameOptions& game_options);

  void GetSuccessors(const GameState& state, std::vector<GameState>* succ);

 private:
  class GameStateHasher {
   public:
    size_t operator()(const GameState& state) const;
  };

  typedef base::hash_map<GameState,  // NOLINT(build/include_what_you_use)
                         std::vector<GameState>,
                         GameStateHasher> SuccessorCache;

  void GetPlaceSuccessors(const GameState& state, std::vector<GameState>* succ);
  void GetMoveSuccessors(const GameState& state, std::vector<GameState>* succ);

  const game::GameOptions game_options_;
  SuccessorCache cache_;

  DISALLOW_COPY_AND_ASSIGN(GameStateTree);
};

}  // namespace ai

#endif  // AI_GAME_STATE_TREE_H_

