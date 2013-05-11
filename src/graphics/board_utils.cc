// Copyright (c) 2013 Cristian Patrasciuc. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "graphics/board_utils.h"

#include "game/board.h"
#include "game/board_location.h"

#include "OGRE/OgreVector3.h"


namespace graphics {

Ogre::Vector3 BoardLocationTo3DCoord(const game::BoardLocation& location,
                                     const game::Board& board) {
  const int delta = board.size() / 2;
  const double scale = board.size() / 2;
  return Ogre::Vector3((location.line() - delta) / scale,
                       (location.column() - delta) / scale,
                       0.0f);
}

}  // namespace graphics
