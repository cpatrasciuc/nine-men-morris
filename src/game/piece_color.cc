// Copyright (c) 2012 Cristian Patrasciuc. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "game/piece_color.h"

#include "base/log.h"

namespace game {

PieceColor GetOpponent(const PieceColor& color) {
  DCHECK(color != NO_COLOR);
  return color == WHITE_COLOR ? BLACK_COLOR : WHITE_COLOR;
}

}  // namespace game
