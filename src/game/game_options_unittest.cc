// Copyright (c) 2013 Cristian Patrasciuc. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "game/game_options.h"
#include "gtest/gtest.h"

namespace game {
namespace {

TEST(GameOptions, Equality) {
  GameOptions op1;
  GameOptions op2(op1);
  EXPECT_EQ(op2, op1);
  op2.set_jumps_allowed(!op2.jumps_allowed());
  EXPECT_NE(op2, op1);
  op2.set_jumps_allowed(!op2.jumps_allowed());
  EXPECT_EQ(op2, op1);
}

}  // anonymous namespace
}  // namespace game
