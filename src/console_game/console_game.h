// Copyright (c) 2012 Cristian Patrasciuc. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef CONSOLE_GAME_CONSOLE_GAME_H_
#define CONSOLE_GAME_CONSOLE_GAME_H_

#include <memory>
#include <string>

#include "base/basic_macros.h"
#include "base/ptr/scoped_ptr.h"
#include "console_game/board_renderer.h"
#include "console_game/console_game_export.h"
#include "game/game.h"

namespace game {
class GameOptions;
}

namespace console_game {

class CommandHandler;
class Player;

class CONSOLE_GAME_EXPORT ConsoleGame {
 public:
  ConsoleGame(const game::GameOptions& options,
              std::auto_ptr<Player> white_player,
              std::auto_ptr<Player> black_player);
  ~ConsoleGame();

  // Draws the current state of the game board.
  void Draw();

  // Runs the game loop that continously calls Draw() and then waits for a user
  // command. The method exits when the user enters the 'q' (quit) command or
  // the game is over.
  void Run();

  // Quit the game after the current player finishes its move.
  void Quit() { should_quit_ = true; }

 private:
  // The game model.
  game::Game game_;

  // The renderer responsible for drawing the game board.
  BoardRenderer board_renderer_;

  // The two Player instances used to query for actions during the game.
  base::ptr::scoped_ptr<Player> white_player_;
  base::ptr::scoped_ptr<Player> black_player_;

  // This flag is set to true using the Quit() method by one of the players if
  // it wants to exit the game.
  bool should_quit_;

  DISALLOW_COPY_AND_ASSIGN(ConsoleGame);
};

}  // namespace console_game

#endif  // CONSOLE_GAME_CONSOLE_GAME_H_

