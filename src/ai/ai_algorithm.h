// Copyright (c) 2013 Cristian Patrasciuc. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef AI_AI_ALGORITHM_H_
#define AI_AI_ALGORITHM_H_

#include "ai/ai_export.h"
#include "base/basic_macros.h"
#include "game/player_action.h"

namespace game {
class Game;
}

namespace ai {

class AI_EXPORT AIAlgorithm {
 public:
  virtual ~AIAlgorithm();

  // This method should return the next move that the current player should
  // perform in the situation given by the |game_model| argument.
  virtual game::PlayerAction GetNextAction(const game::Game& game_model) = 0;

 protected:
  AIAlgorithm();

 private:
  DISALLOW_COPY_AND_ASSIGN(AIAlgorithm);
};

}  // namespace ai

#endif  // AI_AI_ALGORITHM_H_

