// Copyright (c) 2013 Cristian Patrasciuc. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef GRAPHICS_BOARD_UTILS_H_
#define GRAPHICS_BOARD_UTILS_H_

#include "graphics/graphics_export.h"

#include "OGRE/OgreVector3.h"

namespace game {
class Board;
class BoardLocation;
}

namespace graphics {

// Generates the corresponding position in the 3D space for the given board
// location. The coordinates are scaled such that the top right location will
// always be at (1.0, 1.0) and the bottom left location will always be at (-1.0,
// -1.0). The third coordinate is always 0.0.
GRAPHICS_EXPORT Ogre::Vector3 BoardLocationTo3DCoord(
    const game::BoardLocation& location, const game::Board& board);

}  // namespace graphics

#endif  // GRAPHICS_BOARD_UTILS_H_

