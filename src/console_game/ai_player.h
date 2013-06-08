// Copyright (c) 2013 Cristian Patrasciuc. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef CONSOLE_GAME_AI_PLAYER_H_
#define CONSOLE_GAME_AI_PLAYER_H_

#include <memory>
#include <string>

#include "base/basic_macros.h"
#include "base/ptr/scoped_ptr.h"
#include "console_game/console_game_export.h"
#include "console_game/player.h"

namespace ai {
class AIAlgorithm;
}

namespace game {
class Game;
}

namespace console_game {

// Whenever it is asked to perform a move, the AI player simply forwards the
// request to the given |AIAlgorithm|.
class CONSOLE_GAME_EXPORT AIPlayer : public Player {
 public:
  AIPlayer(const std::string& name, std::auto_ptr<ai::AIAlgorithm> algorithm);

 private:
  // Player interface
  virtual std::string GetNextAction(game::Game* game_model);

  // Stores a pointer to the AIAlgorithm used to get the next action.
  base::ptr::scoped_ptr<ai::AIAlgorithm> algorithm_;

  DISALLOW_COPY_AND_ASSIGN(AIPlayer);
};

}  // namespace console_game

#endif  // CONSOLE_GAME_AI_PLAYER_H_

