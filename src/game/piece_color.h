// Copyright (c) 2012 Cristian Patrasciuc. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef GAME_PIECE_COLOR_H_
#define GAME_PIECE_COLOR_H_

#include "game/game_export.h"

namespace game {

// This enum is used to describe the state of each location on the board. It
// contains one value for each player (white and black) and an additional
// value to designate empty spots.
enum PieceColor {
  NO_COLOR,
  WHITE_COLOR,
  BLACK_COLOR
};

// Utility method that can be used to get the opponent of a given color (i.e.
// move back and forth between WHITE and BLACK). The argument must not be equal
// to NO_COLOR.
GAME_EXPORT PieceColor GetOpponent(const PieceColor& color);

}  // namespace game

#endif  // GAME_PIECE_COLOR_H_

