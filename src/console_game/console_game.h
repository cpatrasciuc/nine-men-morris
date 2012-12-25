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

  void Draw();

  void Run();

 private:
  std::string ProcessCommand(const std::string& command);

  game::Game game_;

  DISALLOW_COPY_AND_ASSIGN(ConsoleGame);
};

}  // namespace console_game

#endif  // CONSOLE_GAME_CONSOLE_GAME_H_

