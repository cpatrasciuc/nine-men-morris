// Copyright (c) 2013 Cristian Patrasciuc. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "console_game/ai_player.h"

#include <memory>
#include <sstream>
#include <string>

#include "ai/ai_algorithm.h"
#include "game/game.h"
#include "game/player_action.h"

namespace console_game {

AIPlayer::AIPlayer(const std::string& name,
                   std::auto_ptr<ai::AIAlgorithm> algorithm)
    : Player(name), algorithm_(algorithm.release()) {
}

std::string AIPlayer::GetNextAction(game::Game* game_model) {
  const game::PlayerAction action(algorithm_->GetNextAction(*game_model));
  game_model->ExecutePlayerAction(action);
  std::ostringstream msg;
  msg << name() << " ";
  switch (action.type()) {
    case game::PlayerAction::MOVE_PIECE:
      msg << "moved from " << action.source() << " to " << action.destination();
      break;
    case game::PlayerAction::REMOVE_PIECE:
      msg << "removed a piece from " << action.source();
      break;
    case game::PlayerAction::PLACE_PIECE:
      msg << "placed a piece to " << action.destination();
      break;
  }
  msg << ".";
  return msg.str();
}

}  // namespace console_game
