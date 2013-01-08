// Copyright (c) 2012 Cristian Patrasciuc. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef CONSOLE_GAME_CONSOLE_GAME_H_
#define CONSOLE_GAME_CONSOLE_GAME_H_

#include <map>
#include <memory>
#include <string>

#include "base/basic_macros.h"
#include "base/ptr/scoped_ptr.h"
#include "game/game.h"

namespace game {
class GameOptions;
}

namespace console_game {

class CommandHandler;

class ConsoleGame {
 public:
  explicit ConsoleGame(const game::GameOptions& options);
  ~ConsoleGame();

  // Register a new command handler. This handler's ProcessCommand() method will
  // be called whenever a command of the form "command_type [args]" is
  // entered by the user and command_type is found in the vector returned by the
  // handler's SupportedCommandTypes() method.
  void AddCommandHandler(std::auto_ptr<CommandHandler> handler);

  void AddDefaultCommandHandlers();

  // Draws the current state of the game board.
  void Draw();

  // Runs the game loop that continously calls Draw() and then waits for a user
  // command. The method exits when the user enters the 'q' (quit) command or
  // the game is over.
  void Run();

 private:
  // The game model
  game::Game game_;

  // A mapping from command types to command handlers. To add a new pair use the
  // AddCommandHandler() method.
  std::map<std::string, CommandHandler*> command_handlers_;

  DISALLOW_COPY_AND_ASSIGN(ConsoleGame);
};

}  // namespace console_game

#endif  // CONSOLE_GAME_CONSOLE_GAME_H_

