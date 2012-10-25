// Copyright (c) 2012 Cristian Patrasciuc. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef GAME_BOARD_H_
#define GAME_BOARD_H_

#include <map>

#include "base/basic_macros.h"
#include "game/board_location.h"
#include "game/game_export.h"

namespace game {

class GAME_EXPORT Board {
 public:
  // This enum is used to describe the state of each location on the board. It
  // contains one value for each player (white and black) and an additional
  // value to designate empty spots.
  enum PieceColor {
    NO_COLOR,
    WHITE_COLOR,
    BLACK_COLOR
  };

  explicit Board(int size = 7);

  int size() const { return size_; }

  // This method is used to verify if a given location is valid, i.e. it can be
  // used to place a piece on it or move a piece to it.
  bool IsValidLocation(const BoardLocation& location) const;

 private:
  int size_;

  std::map<BoardLocation, PieceColor> pieces_;

  DISALLOW_COPY_AND_ASSIGN(Board);
};

}  // namespace game

#endif  // GAME_BOARD_H_

