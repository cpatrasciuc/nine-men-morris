// Copyright (c) 2013 Cristian Patrasciuc. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef CONSOLE_GAME_PLAYER_H_
#define CONSOLE_GAME_PLAYER_H_

#include <string>

#include "base/basic_macros.h"
#include "console_game/console_game_export.h"
#include "game/piece_color.h"

namespace game {
class Game;
}

namespace console_game {

class ConsoleGame;

class CONSOLE_GAME_EXPORT Player {
 public:
  explicit Player(const std::string& name);
  virtual ~Player();

  // Add this Player to an instance of ConsoleGame. The second argument
  // represents the color associated to this Player in the game.
  void Initialize(ConsoleGame* game_ptr, const game::PieceColor color);

  const std::string& name() const { return name_; }

  ConsoleGame* current_game() const { return current_game_; }

  // The color of the player in its current associated game. The result is only
  // valid after the Player instance was added to a ConsoleGame instance using
  // the set_current_game() method. Before this, the method returns NO_COLOR.
  const game::PieceColor color() const { return color_; }

  // This method is used to query this Player for the next game action that it
  // wants to take. The method should perform the required changes on the game
  // model object and return a string that represents a status message of the
  // last action. This string is used to provide feedback in the user interface.
  //
  // TODO(console_game): Enhance the interface here. Don't expose the whole game
  // model, since one player might perform actions for the other color too.
  virtual std::string GetNextAction(game::Game* game_model) = 0;

 private:
  const std::string name_;
  game::PieceColor color_;
  ConsoleGame* current_game_;

  DISALLOW_COPY_AND_ASSIGN(Player);
};

}  // namespace console_game

#endif  // CONSOLE_GAME_PLAYER_H_

