// Copyright (c) 2013 Cristian Patrasciuc. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef CONSOLE_GAME_HUMAN_PLAYER_H_
#define CONSOLE_GAME_HUMAN_PLAYER_H_

#include <map>
#include <memory>
#include <string>

#include "base/basic_macros.h"
#include "console_game/command_handler.h"
#include "console_game/console_game_export.h"
#include "console_game/player.h"

namespace game {
class Game;
}

namespace console_game {

class CONSOLE_GAME_EXPORT HumanPlayer : public Player {
 public:
  explicit HumanPlayer(const std::string& name);
  virtual ~HumanPlayer();

  // Register a new command handler. This handler's ProcessCommand() method will
  // be called whenever a command of the form "command_type [args]" is
  // entered by the user and command_type is found in the vector returned by the
  // handler's SupportedCommandTypes() method.
  void RegisterCommandHandler(std::auto_ptr<CommandHandler> handler);

  // Adds default handlers for player actions and saving the game to a file. It
  // should be used in all scenarios that don't imply custom handling of the
  // above mentioned commands.
  void SetupDefaultCommandHandlers();

 private:
  // Player interface
  //
  // First, it reads one line from standard input. Then, it determines the
  // command type from the argument and invokes the ProcessCommand() method of
  // the corresponding command handler. If there is no handler that can be used
  // for this command/command type, nothing is done. The result is a status
  // message that can be used to provide feedback to the user.
  virtual std::string GetNextAction(game::Game* game_model);

  // A mapping from command types to command handlers. To add a new pair use the
  // AddCommandHandler() method.
  std::map<std::string, CommandHandler*> command_handlers_;

  DISALLOW_COPY_AND_ASSIGN(HumanPlayer);
};

}  // namespace console_game

#endif  // CONSOLE_GAME_HUMAN_PLAYER_H_

