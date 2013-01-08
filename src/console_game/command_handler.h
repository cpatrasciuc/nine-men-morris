// Copyright (c) 2012 Cristian Patrasciuc. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef CONSOLE_GAME_COMMAND_HANDLER_H_
#define CONSOLE_GAME_COMMAND_HANDLER_H_

#include <string>
#include <vector>

#include "base/basic_macros.h"

namespace game {
class Game;
}

namespace console_game {

// This class can be used to add custom commands to a console game instance.
// One can create a sub-class of CommandHandler, implement the ProcessCommand()
// method and register the new handler to a game by specifying the command types
// that are supported through the SupportedCommandTypes() method.
//
// For example, if one has:
//   class MyCmdHandler : public CommandHandler {
//     ...
//    public:
//     std::string ProcessCommand(....) {
//       // Do something
//     }
//
//     std::vector<std::string> SupportedCommandTypes() const {
//       std::vector<std::string> result;
//       result.push_back("mycmd");
//       return result;
//     }
//     ...
//   }
//
// An instance of MyCmdHandler can be registered to an instance of ConsoleGame
// as follows:
//   std::auto_ptr<CommandHandler> handler(new MyCmdHandler);
//   console_game.AddCommandHandler(handler);
//
// This handler will be called to process any command that starts with "mycmd".
// Examples:
//   mycmd
//   mycmd a0 b1
//   mycmd and some messages
//   mycmd /path/to/a/file
//
class CommandHandler {
 public:
  CommandHandler();
  virtual ~CommandHandler();

  // The list of command types (i.e. starting tokens) supported by this handler.
  // NOTE:
  //   Each element of the vector:
  //     - must not contain whitespace characters.
  //     - will be treated in a case insensitive way.
  virtual std::vector<std::string> SupportedCommandTypes() const = 0;

  // After this handler is registered to a game to handle a given command type,
  // the game will in turn call this method every time the user types a command
  // starting with the specified type.
  // |command_type| will contain the command type that triggered this call.
  // |args| contain the rest of the command line entered by the user.
  // |game| is the game model on which the command should operate.
  // The return value should be the status message that will be displayed to the
  // user after the command is processed.
  virtual std::string ProcessCommand(const std::string& command_type,
                                     const std::string& args,
                                     game::Game* const game_model) = 0;

 private:
  DISALLOW_COPY_AND_ASSIGN(CommandHandler);
};

// This is the default command handler that handles the MOVE, PLACE and REMOVE
// commands. For convenience, the verbs for these commands can be omitted. The
// command type will be deduced from the game model.
class DefaultCommandHandler : public CommandHandler {
 public:
  DefaultCommandHandler();
  virtual ~DefaultCommandHandler();

  virtual std::vector<std::string> SupportedCommandTypes() const;

  virtual std::string ProcessCommand(const std::string& command_type,
                                     const std::string& args,
                                     game::Game* const game_model);
};

}  // namespace console_game

#endif  // CONSOLE_GAME_COMMAND_HANDLER_H_

