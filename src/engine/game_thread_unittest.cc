// Copyright (c) 2012 Cristian Patrasciuc. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "base/threading/thread.h"
#include "engine/game_thread.h"
#include "gtest/gtest.h"

namespace engine {
namespace {

TEST(GameThread, Basic) {
  GameThread::SetUpGameThreads();
  for (int id = 0; id < GameThread::ID_COUNT; ++id) {
    GameThread::Get(static_cast<GameThread::ID>(id)).QuitWhenIdle();
  }
  GameThread::JoinGameThreads();
}

}  // anonymous namespace
}  // namespace engine
