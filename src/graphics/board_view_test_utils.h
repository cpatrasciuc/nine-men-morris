// Copyright (c) 2013 Cristian Patrasciuc. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef GRAPHICS_BOARD_VIEW_TEST_UTILS_H_
#define GRAPHICS_BOARD_VIEW_TEST_UTILS_H_

namespace game {
class BoardLocation;
}

namespace Ogre {
class Vector3;
}

namespace graphics {

class BoardView;

// Gets the 3D coordinates corresponding to a BoardLcoation in the 3D space of
// the given BoardView.
const Ogre::Vector3& Get3DPosition(const BoardView& view,
                                   const game::BoardLocation& location);

// Sends a series of mouse events to the |view| instance, in order to simulate
// a user click on the center of the board |location| given as argument.
void ClickOnLocation(BoardView* view, const game::BoardLocation& location);

// Sends a series of mouse events to the |view| instance, in order to simulate
// a user click at screen coordinates (x, y). |x| and |y| must be between 0.0
// and 1.0.
void ClickAtScreenCoords(BoardView* view, double screen_x, double screen_y);

}  // namespace graphics

#endif  // GRAPHICS_BOARD_VIEW_TEST_UTILS_H_

