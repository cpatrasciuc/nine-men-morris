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

namespace game {
class GameOptions;
}

namespace ai {

// This class stores a partially constructed game tree. It allows the client to
// obtain the list of successors for a given game state and caches the result
// for subsequent calls.
class AI_EXPORT GameStateTree {
 public:
  explicit GameStateTree(const game::GameOptions& game_options);

  // Appends to the |succ| vector all the successors states of |state|. A state
  // |s| is considered a successor of |state| if |s| can be reached from
  // |state| by performing one or two valid moves (as the current player) under
  // the rules given by the |game_options_|. If a PLACE_PIECE or MOVE_PIECE
  // action, denoted by A, closes a mill, the successor states will include
  // all the states that are obtained by performing A followed by a valid remove
  // action.
  // The method does not check whether |state| is a game-over state or not.
  // NOTE: If |succ| is not empty, its content won't be deleted.
  void GetSuccessors(const GameState& state, std::vector<GameState>* succ);

 private:
  typedef base::hash_map<GameState,  // NOLINT(build/include_what_you_use)
                         std::vector<GameState>,
                         GameStateHasher> SuccessorMap;

  // Get successor states that are obtained by performing a valid PLACE_PIECE
  // action in |state|. If a PLACE_PIECE action closes a mill, all the valid
  // PLACE_PIECE + REMOVE_PIECE combinations are appended to |succ|.
  void GetPlaceSuccessors(const GameState& state, std::vector<GameState>* succ);

  // Get successor states that are obtained by performing a valid MOVE_PIECE
  // action in |state|. If one MOVE_PIECE action closes a mill, all the valid
  // MOVE_PIECE + REMOVE_PIECE combinations are appended to |succ|.
  void GetMoveSuccessors(const GameState& state, std::vector<GameState>* succ);

  const game::GameOptions& game_options_;
  SuccessorMap tree_;

  DISALLOW_COPY_AND_ASSIGN(GameStateTree);
};

}  // namespace ai

#endif  // AI_GAME_STATE_TREE_H_

