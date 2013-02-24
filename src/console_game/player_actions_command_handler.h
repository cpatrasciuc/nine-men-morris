// Copyright (c) 2013 Cristian Patrasciuc. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef CONSOLE_GAME_PLAYER_ACTIONS_COMMAND_HANDLER_H_
#define CONSOLE_GAME_PLAYER_ACTIONS_COMMAND_HANDLER_H_

#include <string>
#include <vector>

#include "console_game/command_handler.h"
#include "console_game/console_game_export.h"

namespace game {
class Game;
}

namespace console_game {

// This is the default command handler that handles the MOVE, PLACE and REMOVE
// commands. For convenience, the verbs for these commands can be omitted. The
// command type will be deduced from the game model.
class CONSOLE_GAME_EXPORT PlayerActionsCommandHandler : public CommandHandler {
 public:
  PlayerActionsCommandHandler();
  virtual ~PlayerActionsCommandHandler();

 private:
  // CommandHandler interface
  virtual std::vector<std::string> SupportedCommandTypes() const;
  virtual bool ProcessCommand(const std::string& command_type,
                              const std::string& args,
                              game::Game* game_model);
  virtual void GetHelpMessage(const std::string& command_type,
                              std::string* format,
                              std::string* usage);
};

}  // namespace console_game

#endif  // CONSOLE_GAME_PLAYER_ACTIONS_COMMAND_HANDLER_H_

