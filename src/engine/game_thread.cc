// Copyright (c) 2012 Cristian Patrasciuc. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "engine/game_thread.h"

#include <string>

#include "base/log.h"
#include "base/threading/thread.h"

namespace engine {

using base::threading::Thread;

std::string GetThreadNameFromID(GameThread::ID id) {
  switch (id) {
    case GameThread::UI:
      return "UI";
    case GameThread::WHITE_PLAYER:
      return "WHITE PLAYER";
    case GameThread::BLACK_PLAYER:
      return "BLACK PLAYER";
    case GameThread::CONTROLLER:
      return "CONTROLLER";
    case GameThread::ID_COUNT:
      NOTREACHED();
  }
  NOTREACHED();
  return std::string();
}

// static
void GameThread::SetUpGameThreads() {
  DCHECK(game_threads.empty()) << "Game threads are already set up";
  for (int id = 0; id < ID_COUNT; ++id) {
    game_threads.push_back(
        new Thread(GetThreadNameFromID(static_cast<GameThread::ID>(id))));
    game_threads[id]->Start();
  }
}

// static
void GameThread::JoinGameThreads() {
  for (int id = 0; id < ID_COUNT; ++id) {
    if (game_threads[id] && game_threads[id]->is_running()) {
      game_threads[id]->Join();
    }
  }
  game_threads.clear();
}

// static
Thread& GameThread::Get(ID identifier) {
  DCHECK(game_threads.size() == ID_COUNT) << "Game threads are not set up";
  Thread* thread = game_threads[identifier];
  DCHECK(thread) << GetThreadNameFromID(identifier);
  return *thread;
}

// static
bool GameThread::CurrentlyOn(ID identifier) {
  return Thread::CurrentlyOn(game_threads[identifier]);
}

// static
std::vector<Thread*> GameThread::game_threads;

}  // namespace engine
