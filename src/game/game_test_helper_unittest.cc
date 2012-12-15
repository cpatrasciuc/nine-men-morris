// Copyright (c) 2012 Cristian Patrasciuc. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include <memory>

#include "game/game.h"
#include "game/game_test_helper.h"
#include "gtest/gtest.h"

namespace game {
namespace {

TEST(GameTestHelper, Basic) {
  std::auto_ptr<Game> game = LoadSavedGameForTests("empty_game_3");
  ASSERT_TRUE(game.get());
}

}  // anonymous namespace
}  // namespace game
