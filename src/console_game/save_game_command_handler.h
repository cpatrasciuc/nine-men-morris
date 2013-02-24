// Copyright (c) 2012 Cristian Patrasciuc. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef CONSOLE_GAME_SAVE_GAME_COMMAND_HANDLER_H_
#define CONSOLE_GAME_SAVE_GAME_COMMAND_HANDLER_H_

#include <string>
#include <vector>

#include "console_game/command_handler.h"
#include "console_game/console_game_export.h"

namespace game {
class Game;
}

namespace console_game {

class CONSOLE_GAME_EXPORT SaveGameCommandHandler : public CommandHandler {
 public:
  SaveGameCommandHandler();
  virtual ~SaveGameCommandHandler();

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

#endif  // CONSOLE_GAME_SAVE_GAME_COMMAND_HANDLER_H_

