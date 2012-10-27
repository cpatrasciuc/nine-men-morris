// Copyright (c) 2012 Cristian Patrasciuc. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include <algorithm>
#include <map>
#include <set>
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
  EXPECT_FALSE(board.IsValidLocation(BoardLocation(10, 10)));
  EXPECT_FALSE(board.IsValidLocation(BoardLocation(-10, -10)));
}

TEST(Board, Adjacency) {
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
  std::map<BoardLocation, std::set<BoardLocation> > adjacency_map;
  for (size_t i = 0; i < arraysize(edges); i += 4) {
    BoardLocation b1(edges[i], edges[i + 1]);
    BoardLocation b2(edges[i + 2], edges[i + 3]);
    EXPECT_TRUE(board.IsAdjacent(b1, b2)) << i / 4;
    adjacency_map[b1].insert(b2);
    EXPECT_TRUE(board.IsAdjacent(b2, b1)) << i / 4;
    adjacency_map[b2].insert(b1);
  }

  for (int x1 = 0; x1 < board.size(); ++x1) {
    for (int y1 = 0; y1 < board.size(); ++y1) {
      BoardLocation b1(x1, y1);
      if (board.IsValidLocation(b1)) {
        std::set<BoardLocation>& s = adjacency_map[b1];
        std::vector<BoardLocation> v;
        board.GetAdjacentLocations(b1, &v);
        std::set<BoardLocation> adjacent_locations(v.begin(), v.end());
        EXPECT_EQ(s, adjacent_locations);

        for (int x2 = 0; x2 < board.size(); ++x2) {
          for (int y2 = 0; y2 < board.size(); ++y2) {
            BoardLocation b2(x2, y2);
            if (board.IsValidLocation(b1) && board.IsValidLocation(b2)) {
              if (adjacency_map.find(b1) != adjacency_map.end()) {
                continue;
              }
              EXPECT_FALSE(board.IsAdjacent(b1, b2)) << b1 << " " << b2;
            }
          }
        }
      }
    }
  }
}

TEST(Board, AddGetRemove) {
  Board board(5);
  BoardLocation loc(0, 0);
  EXPECT_EQ(Board::NO_COLOR, board.GetPieceAt(loc));
  EXPECT_FALSE(board.RemovePiece(loc));
  EXPECT_FALSE(board.AddPiece(loc, Board::NO_COLOR));
  EXPECT_TRUE(board.AddPiece(loc, Board::WHITE_COLOR));
  EXPECT_EQ(Board::WHITE_COLOR, board.GetPieceAt(loc));
  EXPECT_FALSE(board.AddPiece(loc, Board::BLACK_COLOR));
  EXPECT_FALSE(board.AddPiece(loc, Board::WHITE_COLOR));
  EXPECT_TRUE(board.RemovePiece(loc));
  EXPECT_FALSE(board.RemovePiece(loc));
  EXPECT_EQ(Board::NO_COLOR, board.GetPieceAt(loc));
  EXPECT_TRUE(board.AddPiece(loc, Board::BLACK_COLOR));
  EXPECT_FALSE(board.AddPiece(BoardLocation(10, 10), Board::BLACK_COLOR));
}

TEST(Board, MovePiece) {
  Board board;
  const BoardLocation old_location(0, 0);
  const BoardLocation new_locations[] = {
    // Adjacent move
    BoardLocation(0, 3),
    // Jump
    BoardLocation(board.size() - 1, board.size() - 1)
  };
  const Board::PieceColor color = Board::WHITE_COLOR;
  board.AddPiece(old_location, color);

  for (size_t i = 0; i < arraysize(new_locations); ++i) {
    const BoardLocation& new_location = new_locations[i];
    EXPECT_EQ(color, board.GetPieceAt(old_location));
    EXPECT_EQ(Board::NO_COLOR, board.GetPieceAt(new_location));
    board.MovePiece(old_location, new_location);
    EXPECT_EQ(Board::NO_COLOR, board.GetPieceAt(old_location));
    EXPECT_EQ(color, board.GetPieceAt(new_location));
    board.MovePiece(new_location, old_location);
  }
  // TODO(board): Add piece count
}

TEST(BoardDeathTest, MovePiece) {
  ::testing::FLAGS_gtest_death_test_style = "threadsafe";
  Board board;
  const BoardLocation old_location(0, 0);
  const BoardLocation new_location(0, 3);
  ASSERT_DEATH(board.MovePiece(old_location, new_location), "");
  board.AddPiece(old_location, Board::WHITE_COLOR);
  board.AddPiece(new_location, Board::BLACK_COLOR);
  ASSERT_DEATH(board.MovePiece(old_location, new_location), "");
  ASSERT_DEATH(board.MovePiece(old_location, old_location), "");
}

TEST(BoardDeathTest, UseInvalidLocations) {
  ::testing::FLAGS_gtest_death_test_style = "threadsafe";
  Board board;
  const BoardLocation invalid(-1, -1);
  const BoardLocation valid(0, 0);
  ASSERT_DEATH(board.GetAdjacentLocations(invalid, NULL), "");
  ASSERT_DEATH(board.GetPieceAt(invalid), "");
  ASSERT_DEATH(board.IsAdjacent(invalid, valid), "");
  ASSERT_DEATH(board.IsAdjacent(valid, invalid), "");
  ASSERT_DEATH(board.MovePiece(invalid, valid), "");
  ASSERT_DEATH(board.MovePiece(valid, invalid), "");
}

}  // anonymous namespace
}  // namespace game
