// Copyright (c) 2012 Cristian Patrasciuc. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "game/board.h"

#include "game/board_location.h"

namespace game {

Board::Board(int size) : size_(size) {
  // TODO(board): Add debug constraints for size
}

bool Board::IsValidLocation(const BoardLocation& location) const {
  // First check that the location is inside the board
  if (location.line() >= size_ || location.column() >= size_) {
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

}  // namespace game
