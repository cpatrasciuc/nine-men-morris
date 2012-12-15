// Copyright (c) 2012 Cristian Patrasciuc. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "game/board.h"

#include <algorithm>
#include <cstdlib>
#include <functional>
#include <map>
#include <utility>

#include "base/basic_macros.h"
#include "base/log.h"
#include "game/board_location.h"
#include "game/game_options.h"

using std::map;
using std::vector;

namespace game {

namespace {

int GetBoardSizeFromGameType(GameOptions::GameType type) {
  switch (type) {
    case GameOptions::THREE_MEN_MORRIS:
      return 3;
    case GameOptions::SIX_MEN_MORRIS:
      return 5;
    case GameOptions::NINE_MEN_MORRIS:
      return 7;
  }
  NOTREACHED();
  return -1;
}

class PieceColorEqualTo
    : public std::unary_function<
        const std::pair<BoardLocation, Board::PieceColor>&, bool> {
 public:
  explicit PieceColorEqualTo(const Board::PieceColor color) : color_(color) {}

  bool operator()(const std::pair<BoardLocation, Board::PieceColor>& p) const {
    return p.second == color_;
  }

 private:
  Board::PieceColor color_;
};

}  // anonymous namespace

Board::Board(GameOptions::GameType type)
    : size_(GetBoardSizeFromGameType(type)), pieces_() {}

int Board::GetPieceCountByColor(PieceColor color) const {
  DCHECK(color != NO_COLOR);
  return std::count_if(pieces_.begin(),
                       pieces_.end(),
                       PieceColorEqualTo(color));
}

bool Board::IsValidLocation(const BoardLocation& location) const {
  // First check that the location is inside the board
  if (location.line() >= size_ || location.column() >= size_ ||
      location.line() < 0 || location.column() < 0) {
    return false;
  }
  // Special case for size_ == 3, where all locations are valid
  if (size_ == 3) {
    return true;
  }
  int l = location.line();
  int c = location.column();
  // Only the middle of the board is not allowed for the center line
  if (l == size_ / 2) {
    return (c != size_ / 2);
  }
  // Handle symmetry
  if (l > size_ / 2) {
    l = size_ - l - 1;
  }
  // Location from the middle column are allowed for all lines < size_ / 2
  if (c == size_ / 2) {
    return true;
  }
  // Handle symmetry
  if (c > size_ / 2) {
    c = size_ - c - 1;
  }
  // Check if the location is on the diagonal
  return l == c;
}

bool Board::IsAdjacent(const BoardLocation& b1, const BoardLocation& b2) const {
  DCHECK(IsValidLocation(b1));
  DCHECK(IsValidLocation(b2));
  int x = 0;
  int y = 0;
  int common_coordinate = 0;
  if (b1.line() == b2.line()) {
    x = b1.column();
    y = b2.column();
    common_coordinate = b1.line();
  } else if (b1.column() == b2.column()) {
    x = b1.line();
    y = b2.line();
    common_coordinate = b1.column();
  }
  if (common_coordinate > size_ / 2) {
    common_coordinate = size_ - common_coordinate - 1;
  }
  if (abs(x - y) == GetStep(common_coordinate)) {
    return true;
  }
  return false;
}

void Board::GetAdjacentLocations(const BoardLocation& loc,
    vector<BoardLocation>* adjacent_locations) const {
  DCHECK(IsValidLocation(loc));
  int horizontal_step = GetStep(loc.column());
  int vertical_step = GetStep(loc.line());
  int dx[] = { horizontal_step, -horizontal_step, 0, 0 };
  int dy[] = { 0, 0, vertical_step, -vertical_step };
  for (size_t i = 0; i < arraysize(dx); ++i) {
    BoardLocation new_loc(loc.line() + dx[i], loc.column() + dy[i]);
    if (IsValidLocation(new_loc)) {
      adjacent_locations->push_back(new_loc);
    }
  }
}

bool Board::AddPiece(const BoardLocation& location, PieceColor color) {
  DCHECK(color != NO_COLOR);
  if (!IsValidLocation(location)) {
    return false;
  }
  map<BoardLocation, PieceColor>::iterator it = pieces_.find(location);
  if (it != pieces_.end()) {
    return false;
  }
  pieces_.insert(it, std::make_pair(location, color));
  return true;
}

bool Board::RemovePiece(const BoardLocation& location) {
  map<BoardLocation, PieceColor>::iterator it = pieces_.find(location);
  if (it == pieces_.end()) {
    return false;
  }
  pieces_.erase(it);
  return true;
}

Board::PieceColor Board::GetPieceAt(const BoardLocation& location) const {
  DCHECK(IsValidLocation(location));
  map<BoardLocation, PieceColor>::const_iterator it = pieces_.find(location);
  if (it != pieces_.end()) {
    return (*it).second;
  }
  return NO_COLOR;
}

void Board::MovePiece(const BoardLocation& old_loc,
                      const BoardLocation& new_loc) {
  DCHECK(IsValidLocation(old_loc));
  DCHECK(IsValidLocation(new_loc));
  PieceColor color = GetPieceAt(old_loc);
  DCHECK(color != NO_COLOR);
  DCHECK_EQ(NO_COLOR, GetPieceAt(new_loc));
  RemovePiece(old_loc);
  AddPiece(new_loc, color);
}

bool Board::IsPartOfMill(const BoardLocation& location) const {
  PieceColor color = GetPieceAt(location);
  if (color == NO_COLOR) {
    return false;
  }

  int step = GetStep(location.column());
  int dx[] = { -2 * step, -step, 0, step, 2 * step };
  bool horizontal_pieces[arraysize(dx)] = { false };
  for (size_t i = 0; i < arraysize(dx); ++i) {
    BoardLocation loc(location.line() + dx[i], location.column());
    if (IsValidLocation(loc)) {
      if (GetPieceAt(loc) == color) {
        horizontal_pieces[i] = true;
        if (i >= 2) {
          if (horizontal_pieces[i - 1] && horizontal_pieces[i - 2]) {
            return true;
          }
        }
      }
    }
  }

  step = GetStep(location.line());
  int dy[] = { -2 * step, -step, 0, step, 2 * step };
  bool vertical_pieces[arraysize(dy)] = { false };
  for (size_t i = 0; i < arraysize(dy); ++i) {
    BoardLocation loc(location.line(), location.column() + dy[i]);
    if (IsValidLocation(loc)) {
      if (GetPieceAt(loc) == color) {
        vertical_pieces[i] = true;
        if (i >= 2) {
          if (vertical_pieces[i - 1] && vertical_pieces[i - 2]) {
            return true;
          }
        }
      }
    }
  }

  return false;
}

int Board::GetStep(int index) const {
  if (index == size_ / 2) {
    return 1;
  }
  return (size_ - 1 - 2 * index) / 2;
};

}  // namespace game
