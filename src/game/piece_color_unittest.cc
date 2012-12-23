// Copyright (c) 2012 Cristian Patrasciuc. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "game/piece_color.h"
#include "gtest/gtest.h"

#if defined(DEBUG_MODE)
#define MAYBE(testcase) testcase
#else
#define MAYBE(testcase) DISABLED_##testcase
#endif

namespace game {
namespace {

TEST(PieceColor, GetOpponent) {
  EXPECT_EQ(WHITE_COLOR, GetOpponent(BLACK_COLOR));
  EXPECT_EQ(BLACK_COLOR, GetOpponent(WHITE_COLOR));
}

TEST(PieceColorDeathTest, MAYBE(GetOpponentInvalidColor)) {
  ASSERT_DEATH(GetOpponent(NO_COLOR), "");
}

}  // anonymous namespace
}  // namespace game
