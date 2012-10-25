// Copyright (c) 2012 Cristian Patrasciuc. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "game/board_location.h"

namespace game {

BoardLocation::BoardLocation(int line, int column)
    : line_(line), column_(column) {}

bool BoardLocation::operator <(const BoardLocation& other) const {
  return (line_ < other.line_) ? true :
      ((line_ == other.line_) ? (column_ < other.column_) : false);
}

}  // namespace game
