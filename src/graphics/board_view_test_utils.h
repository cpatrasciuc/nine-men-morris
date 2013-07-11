// Copyright (c) 2013 Cristian Patrasciuc. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef GRAPHICS_BOARD_VIEW_TEST_UTILS_H_
#define GRAPHICS_BOARD_VIEW_TEST_UTILS_H_

namespace game {
class BoardLocation;
}

namespace graphics {

class BoardView;

// Sends a series of mouse events to the |view| instance, in order to simulate
// a user click on the center of the board |location| given as argument.
void ClickOnLocation(BoardView* view, const game::BoardLocation& location);

}  // namespace graphics

#endif  // GRAPHICS_BOARD_VIEW_TEST_UTILS_H_

