// Copyright (c) 2012 Cristian Patrasciuc. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include <algorithm>
#include <map>
#include <vector>

#include "base/basic_macros.h"
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

TEST(Board, IsAdjacent) {
  const int edges[] = {
    0, 0, 3, 0,
    0, 0, 0, 3,
    0, 3, 1, 3,
    0, 3, 0, 6,
    0, 6, 3, 6,
    1, 1, 3, 1,
    1, 1, 1, 3,
    1, 3, 2, 3,
    1, 3, 1, 5,
    1, 5, 3, 5,
    2, 2, 3, 2,
    2, 2, 2, 3,
    2, 3, 2, 4,
    2, 4, 3, 4,
    3, 0, 3, 1,
    3, 0, 6, 0,
    3, 1, 3, 2,
    3, 1, 5, 1,
    3, 2, 4, 2,
    3, 4, 3, 5,
    3, 4, 4, 4,
    3, 5, 3, 6,
    3, 5, 5, 5,
    3, 6, 6, 6,
    4, 2, 4, 3,
    4, 3, 4, 4,
    4, 3, 5, 3,
    5, 1, 5, 3,
    5, 3, 5, 5,
    5, 3, 6, 3,
    6, 0, 6, 3,
    6, 3, 6, 6
  };
  Board board(7);
  std::map<BoardLocation, std::vector<BoardLocation> > adjacency_map;
  for (size_t i = 0; i < arraysize(edges); i += 4) {
    BoardLocation b1(edges[i], edges[i + 1]);
    BoardLocation b2(edges[i + 2], edges[i + 3]);
    EXPECT_TRUE(board.IsAdjacent(b1, b2)) << i / 4;
    adjacency_map[b1].push_back(b2);
    EXPECT_TRUE(board.IsAdjacent(b2, b1)) << i / 4;
    adjacency_map[b2].push_back(b1);
  }

  for (int x1 = 0; x1 < board.size(); ++x1) {
    for (int x2 = 0; x2 < board.size(); ++x2) {
      for (int y1 = 0; y1 < board.size(); ++y1) {
        for (int y2 = 0; y2 < board.size(); ++y2) {
          BoardLocation b1(x1, y1);
          BoardLocation b2(x2, y2);
          if (board.IsValidLocation(b1) && board.IsValidLocation(b2)) {
            if (adjacency_map.find(b1) != adjacency_map.end()) {
              std::vector<BoardLocation>& v = adjacency_map[b1];
              if (std::find(v.begin(), v.end(), b2) != v.end()) {
                continue;
              }
            }
            EXPECT_FALSE(board.IsAdjacent(b1, b2)) << b1 << " " << b2;
          }
        }
      }
    }
  }
}

}  // anonymous namespace
}  // namespace game
