// Copyright (c) 2013 Cristian Patrasciuc. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "graphics/ois_test_utils.h"

#include "base/log.h"
#include "graphics/ogre_app.h"

#include "OIS/OISMouse.h"

namespace graphics {

void SimulateClick(OIS::MouseListener* listener,
                   double screen_x,
                   double screen_y,
                   OIS::MouseButtonID button_id) {
  DCHECK(0.0 <= screen_x && screen_x <= 1.0);
  DCHECK(0.0 <= screen_y && screen_y <= 1.0);
  OgreApp& app = OgreApp::Instance();
  const OIS::MouseState& real_mouse = app.mouse().getMouseState();
  OIS::MouseState state(real_mouse);
  state.X.abs = screen_x * state.width;
  state.Y.abs = screen_y * state.height;
  const OIS::MouseEvent move_event(NULL, state);
  listener->mouseMoved(move_event);
  const OIS::MouseEvent press_event(NULL, state);
  listener->mousePressed(press_event, button_id);
  const OIS::MouseEvent release_event(NULL, state);
  listener->mouseReleased(release_event, button_id);
}

void SimulateKeyPress(OIS::KeyListener* listener, OIS::KeyCode key) {
  const OIS::KeyEvent key_event(NULL, key, 0);
  listener->keyPressed(key_event);
  listener->keyReleased(key_event);
}

}  // namespace graphics
