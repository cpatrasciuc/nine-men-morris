// Copyright (c) 2012 Cristian Patrasciuc. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "game/board.h"

#include <cstdlib>
#include <map>

#include "base/basic_macros.h"
#include "base/log.h"
#include "game/board_location.h"

using std::map;
using std::vector;

namespace game {

Board::Board(int size) : size_(size), pieces_() {
  // TODO(board): Add debug constraints for size
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

void Board::GetAdjacentLocations(BoardLocation loc,
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
  if (color == NO_COLOR) {
    return false;
  }
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
  // TODO(board): Check if the piece is not in a mill before removing
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

int Board::GetStep(int index) const {
  if (index == size_ / 2) {
    return 1;
  }
  return (size_ - 1 - 2 * index) / 2;
};

}  // namespace game
