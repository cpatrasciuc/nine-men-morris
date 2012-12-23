// Copyright (c) 2012 Cristian Patrasciuc. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef CONTROLLER_GAME_THREAD_H_
#define CONTROLLER_GAME_THREAD_H_

#include <vector>

namespace base {
namespace threading {
class Thread;
}
}

namespace controller {

class GameThread {
 public:
  enum ID {
    UI,
    WHITE_PLAYER,
    BLACK_PLAYER,
    CONTROLLER,
    ID_COUNT  // Should not be used
  };

  static void SetUpGameThreads();
  static void JoinGameThreads();

  static base::threading::Thread& Get(ID identifier);

  static bool CurrentlyOn(ID identifier);

 private:
  static std::vector<base::threading::Thread*> game_threads;
};

}  // namespace controller

#endif  // CONTROLLER_GAME_THREAD_H_

