// Copyright (c) 2012 Cristian Patrasciuc. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include <map>

#include "game/board_location.h"
#include "gtest/gtest.h"

namespace game {
namespace {

TEST(BoardLocation, Ordering) {
  BoardLocation loc(10, 10);
  EXPECT_TRUE(loc < BoardLocation(20, 10));
  EXPECT_TRUE(loc < BoardLocation(20, 20));
  EXPECT_TRUE(BoardLocation(0, 10) < loc);
  EXPECT_TRUE(BoardLocation(0,  0) < loc);
  EXPECT_FALSE(BoardLocation(10, 10) < loc);
}

TEST(BoardLocation, StdMap) {
  std::map<BoardLocation, bool> m;
  int size = 10;
  for (int i = 0; i < size; ++i) {
    for (int j = 0; j < size; ++j) {
      m[BoardLocation(i, j)] = true;
    }
  }
  EXPECT_EQ(size * size, static_cast<int>(m.size()));

  m.clear();
  m[BoardLocation(10, 10)] = true;
  m[BoardLocation(10, 10)] = false;
  EXPECT_EQ(1U, m.size());
  EXPECT_FALSE(m[BoardLocation(10, 10)]);
}

}  // anonymous namespace
}  // namespace game
