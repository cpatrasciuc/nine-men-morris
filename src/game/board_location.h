// Copyright (c) 2012 Cristian Patrasciuc. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef GAME_BOARD_LOCATION_H_
#define GAME_BOARD_LOCATION_H_

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

 private:
  int line_;
  int column_;
};

}  // namespace game

#endif  // GAME_BOARD_LOCATION_H_

