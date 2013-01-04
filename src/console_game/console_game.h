// Copyright (c) 2012 Cristian Patrasciuc. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef CONSOLE_GAME_CONSOLE_GAME_H_
#define CONSOLE_GAME_CONSOLE_GAME_H_

#include <string>

#include "base/basic_macros.h"
#include "base/ptr/scoped_ptr.h"
#include "game/game.h"

namespace game {
class GameOptions;
}

namespace console_game {

class ConsoleGame {
 public:
  explicit ConsoleGame(const game::GameOptions& options);

  // Draws the current state of the game board.
  void Draw();

  // Runs the game loop that continously calls Draw() and then waits for a user
  // command. The method exits when the user enters the 'q' (quit) command or
  // the game is over.
  void Run();

 private:
  // Handle the latest user command and perform the necessary changes on the
  // |game_| object. The method returns a string that represents the status of
  // the command being run. The result is used to provide feedback to the user.
  std::string ProcessCommand(const std::string& command);

  // The game model
  game::Game game_;

  DISALLOW_COPY_AND_ASSIGN(ConsoleGame);
};

}  // namespace console_game

#endif  // CONSOLE_GAME_CONSOLE_GAME_H_

