// Copyright (c) 2012 Cristian Patrasciuc. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef GAME_BOARD_LOCATION_H_
#define GAME_BOARD_LOCATION_H_

#include <ostream>

#include "game/game_export.h"

namespace game {

// This class represents one location on the game board, specified by line and
// column.
// WARNING: It is not guaranteed that the specified location is a valid one.
//          To verify this use Board::IsValidLocation().
class GAME_EXPORT BoardLocation {
 public:
  BoardLocation(int line, int column);

  int line() const { return line_; }
  int column() const { return column_; }

  // This must be declared so we can use BoardLocation objects as std::map keys.
  bool operator<(const BoardLocation& other) const;

  // Equality and inequality operators used to search for BoardLocations objects
  // in STL containers.
  bool operator==(const BoardLocation& other) const;
  bool operator!=(const BoardLocation& other) const {
    return !(*this == other);
  }

 private:
  int line_;
  int column_;
};

GAME_EXPORT
std::ostream& operator<<(std::ostream& out, const game::BoardLocation& loc);

}  // namespace game

#endif  // GAME_BOARD_LOCATION_H_

