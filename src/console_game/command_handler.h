// Copyright (c) 2012 Cristian Patrasciuc. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef CONSOLE_GAME_COMMAND_HANDLER_H_
#define CONSOLE_GAME_COMMAND_HANDLER_H_

#include <string>

#include "base/basic_macros.h"

namespace game {
class Game;
}

namespace console_game {

// This class can be used to add custom commands to a console game instance.
// One can create a sub-class of CommandHandler, implement the ProcessCommand()
// method and register the new handler to a game by specifying the command name.
//
// For example, if you have:
//   class MyCmdHandler : public CommandHandler {
//     ...
//    public:
//     std::string ProcessCommand(....) {
//       // Do something
//     }
//     ...
//   }
//
// You can register one instance of MyCmdHandler to an instance of ConsoleGame
// as follows:
// TODO(command_handler): Finish documentation
class CommandHandler {
 public:
  CommandHandler();
  virtual ~CommandHandler();

  // After you register this handler to a game to handle a given command type,
  // the game will in turn call this method every time the user types a command
  // starting with the type you specified.
  // |command| will contain the entire command entered by the user.
  // |game| is the game model on which the command should operate.
  // The return value should be the status message that will be displayed to the
  // user after the command is processed.
  virtual std::string ProcessCommand(const std::string& command,
                                     game::Game* game_model) = 0;

 private:
  DISALLOW_COPY_AND_ASSIGN(CommandHandler);
};

// This is the default command handler that handles the MOVE, PLACE and REMOVE
// commands. For convenience, the verbs for these commands can be omitted. The
// command type will be deduced from the game model.
class DefaultCommandHandler {
 public:
  DefaultCommandHandler();
  virtual ~DefaultCommandHandler();

  virtual std::string ProcessCommand(const std::string& command,
                                     game::Game* game_model);
};

}  // namespace console_game

#endif  // CONSOLE_GAME_COMMAND_HANDLER_H_

