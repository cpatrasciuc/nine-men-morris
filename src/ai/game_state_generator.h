// Copyright (c) 2013 Cristian Patrasciuc. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef AI_GAME_STATE_GENERATOR_H_
#define AI_GAME_STATE_GENERATOR_H_

#include <vector>

#include "ai/ai_export.h"

#include "game/game_options.h"

namespace ai {

class GameState;

class AI_EXPORT GameStateGenerator {
 public:
  explicit GameStateGenerator(game::GameOptions::GameType game_type);

  void GetSuccessors(const GameState& state, std::vector<GameState>* succ);

 private:
  void GetPlaceSuccessors(const GameState& state, std::vector<GameState>* succ);
  void GetMoveSuccessors(const GameState& state, std::vector<GameState>* succ);

  game::GameOptions::GameType game_type_;
};

}  // namespace ai

#endif  // AI_GAME_STATE_GENERATOR_H_

