// Copyright (c) 2012 Cristian Patrasciuc. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef GAME_PIECE_COLOR_H_
#define GAME_PIECE_COLOR_H_

namespace game {

// This enum is used to describe the state of each location on the board. It
// contains one value for each player (white and black) and an additional
// value to designate empty spots.
enum PieceColor {
  NO_COLOR,
  WHITE_COLOR,
  BLACK_COLOR
};

}  // namespace game

#endif  // GAME_PIECE_COLOR_H_

