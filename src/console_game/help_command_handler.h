// Copyright (c) 2013 Cristian Patrasciuc. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef CONSOLE_GAME_HELP_COMMAND_HANDLER_H_
#define CONSOLE_GAME_HELP_COMMAND_HANDLER_H_

#include <map>
#include <string>
#include <vector>

#include "console_game/command_handler.h"

namespace Game {
class Game;
}

namespace console_game {

class HelpCommandHandler : public CommandHandler {
 public:
  explicit HelpCommandHandler(
      const std::map<std::string, CommandHandler*>& handlers);
  virtual ~HelpCommandHandler();

  // CommandHandler interface
  virtual std::vector<std::string> SupportedCommandTypes() const;
  virtual std::string ProcessCommand(const std::string& command_type,
                                     const std::string& args,
                                     game::Game* const game_model);
  virtual void GetHelpMessage(const std::string& command_type,
                              std::string* format,
                              std::string* usage);

 private:
  const std::map<std::string, CommandHandler*>& handlers_;
};

}  // namespace console_game

#endif  // CONSOLE_GAME_HELP_COMMAND_HANDLER_H_

