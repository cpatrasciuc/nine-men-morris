// Copyright (c) 2013 Cristian Patrasciuc. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef GRAPHICS_OIS_TEST_UTILS_H_
#define GRAPHICS_OIS_TEST_UTILS_H_

#include "OIS/OISKeyboard.h"
#include "OIS/OISMouse.h"

namespace graphics {

// Create a series of I/O events and pass them to the |listener| in order to
// simulate a mouse click at screen coordinates (screen_x, screen_y). The given
// coordinates must be between 0.0 and 1.0.
void SimulateClick(OIS::MouseListener* listener,
                   double screen_x,
                   double screen_y,
                   OIS::MouseButtonID button_id = OIS::MB_Left);

// Create a series of I/O events and pass them to the |listener| in order to
// simulate pressing the given |key|.
void SimulateKeyPress(OIS::KeyListener* listener, OIS::KeyCode key);

}  // namespace graphics

#endif  // GRAPHICS_OIS_TEST_UTILS_H_

