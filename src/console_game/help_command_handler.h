// Copyright (c) 2013 Cristian Patrasciuc. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef CONSOLE_GAME_HELP_COMMAND_HANDLER_H_
#define CONSOLE_GAME_HELP_COMMAND_HANDLER_H_

#include <iostream>
#include <map>
#include <ostream>
#include <string>
#include <vector>

#include "console_game/command_handler.h"
#include "console_game/console_game_export.h"

namespace game {
class Game;
}

namespace console_game {

class CONSOLE_GAME_EXPORT HelpCommandHandler : public CommandHandler {
 public:
  // The |handlers| argument must contain the command types and their associated
  // command handlers for which a help message will be generated. The optional
  // |out| argument is the stream to which the help message will be printed. If
  // |out| is equal to std::cout, the handler will wait for a key press after
  // displaying the help message.
  explicit HelpCommandHandler(
      const std::map<std::string, CommandHandler*>& handlers,
      std::ostream* out = &std::cout);
  virtual ~HelpCommandHandler();

 private:
  // CommandHandler interface
  virtual std::vector<std::string> SupportedCommandTypes() const;
  virtual bool ProcessCommand(const std::string& command_type,
                              const std::string& args,
                              game::Game* game_model);
  virtual void GetHelpMessage(const std::string& command_type,
                              std::string* format,
                              std::string* usage);

  // Checks if the handler should run in interactive mode (i.e. clean console
  // screen and wait for a key press after displaying the help message).
  bool IsInteractive() const;

  const std::map<std::string, CommandHandler*>& handlers_;
  std::ostream* out_;
};

}  // namespace console_game

#endif  // CONSOLE_GAME_HELP_COMMAND_HANDLER_H_

