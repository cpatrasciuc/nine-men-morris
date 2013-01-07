// Copyright (c) 2012 Cristian Patrasciuc. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef CONSOLE_GAME_SAVE_GAME_COMMAND_HANDLER_H_
#define CONSOLE_GAME_SAVE_GAME_COMMAND_HANDLER_H_

#include <string>

#include "console_game/command_handler.h"

namespace game {
class Game;
}

namespace console_game {

class SaveGameCommandHandler : public CommandHandler {
 public:
  static const char kSaveGameCommandName[];

  SaveGameCommandHandler();
  virtual ~SaveGameCommandHandler();

  virtual std::string ProcessCommand(const std::string& command,
                                     game::Game* game_model);
};

}  // namespace console_game

#endif  // CONSOLE_GAME_SAVE_GAME_COMMAND_HANDLER_H_

