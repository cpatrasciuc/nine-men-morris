// Copyright (c) 2013 Cristian Patrasciuc. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef CONSOLE_GAME_PLAYER_ACTIONS_COMMAND_HANDLER_H_
#define CONSOLE_GAME_PLAYER_ACTIONS_COMMAND_HANDLER_H_

#include <string>
#include <vector>

#include "console_game/command_handler.h"

namespace Game {
class Game;
}

namespace console_game {

// This is the default command handler that handles the MOVE, PLACE and REMOVE
// commands. For convenience, the verbs for these commands can be omitted. The
// command type will be deduced from the game model.
class PlayerActionsCommandHandler : public CommandHandler {
 public:
  PlayerActionsCommandHandler();
  virtual ~PlayerActionsCommandHandler();

  virtual std::vector<std::string> SupportedCommandTypes() const;

  virtual std::string ProcessCommand(const std::string& command_type,
                                     const std::string& args,
                                     game::Game* const game_model);
};

}  // namespace console_game

#endif  // CONSOLE_GAME_PLAYER_ACTIONS_COMMAND_HANDLER_H_

