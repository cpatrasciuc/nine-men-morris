// Copyright (c) 2012 Cristian Patrasciuc. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "game/board.h"
#include "game/board_location.h"
#include "gtest/gtest.h"

namespace game {
namespace {

TEST(Board, IsValidLocation) {
  const int n = 7;
  const int b[n][n] = {
    { 1, 0, 0, 1, 0, 0, 1 },
    { 0, 1, 0, 1, 0, 1, 0 },
    { 0, 0, 1, 1, 1, 0, 0 },
    { 1, 1, 1, 0, 1, 1, 1 },
    { 0, 0, 1, 1, 1, 0, 0 },
    { 0, 1, 0, 1, 0, 1, 0 },
    { 1, 0, 0, 1, 0, 0, 1 }
  };
  Board board(n);
  for (int i = 0; i < n; ++i) {
    for (int j = 0; j < n; ++j) {
      EXPECT_EQ(b[i][j], board.IsValidLocation(BoardLocation(i, j)));
    }
  }
}

}  // anonymous namespace
}  // namespace game
