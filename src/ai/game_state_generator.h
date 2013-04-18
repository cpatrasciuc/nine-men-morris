// Copyright (c) 2013 Cristian Patrasciuc. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef AI_GAME_STATE_GENERATOR_H_
#define AI_GAME_STATE_GENERATOR_H_

#include <vector>

#include "ai/ai_export.h"
#include "base/basic_macros.h"
#include "game/game_options.h"

namespace ai {

class GameState;

class AI_EXPORT GameStateGenerator {
 public:
  explicit GameStateGenerator(const game::GameOptions& game_options);

  void GetSuccessors(const GameState& state, std::vector<GameState>* succ);

 private:
  void GetPlaceSuccessors(const GameState& state, std::vector<GameState>* succ);
  void GetMoveSuccessors(const GameState& state, std::vector<GameState>* succ);

  const game::GameOptions game_options_;

  DISALLOW_COPY_AND_ASSIGN(GameStateGenerator);
};

}  // namespace ai

#endif  // AI_GAME_STATE_GENERATOR_H_

