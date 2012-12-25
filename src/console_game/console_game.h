// Copyright (c) 2012 Cristian Patrasciuc. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef CONSOLE_GAME_CONSOLE_GAME_H_
#define CONSOLE_GAME_CONSOLE_GAME_H_

#include "base/basic_macros.h"

namespace console_game {

class ConsoleGame {
 public:
  ConsoleGame();

  void Run();

 private:
  DISALLOW_COPY_AND_ASSIGN(ConsoleGame);
};

}  // namespace console_game

#endif  // CONSOLE_GAME_CONSOLE_GAME_H_

