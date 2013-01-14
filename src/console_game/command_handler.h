// Copyright (c) 2012 Cristian Patrasciuc. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef CONSOLE_GAME_COMMAND_HANDLER_H_
#define CONSOLE_GAME_COMMAND_HANDLER_H_

#include <string>
#include <vector>

#include "base/basic_macros.h"
#include "console_game/console_game_export.h"

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
class CONSOLE_GAME_EXPORT CommandHandler {
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
  // The return value is |true| if the command was executed successfully;
  // |false| otherwise.
  virtual bool ProcessCommand(const std::string& command_type,
                              const std::string& args,
                              game::Game* game_model) = 0;

  // Method used to obtain the help message for the command types handled by
  // this handler. The |command_type| argument represents the command type for
  // which help is requested. The method should fill the |format| argument with
  // the format of the command and its arguments on one line. The |usage| string
  // should be filled with the detailed explanation on how to use the command.
  virtual void GetHelpMessage(const std::string& command_type,
                              std::string* format,
                              std::string* usage) = 0;

 private:
  DISALLOW_COPY_AND_ASSIGN(CommandHandler);
};

}  // namespace console_game

#endif  // CONSOLE_GAME_COMMAND_HANDLER_H_

