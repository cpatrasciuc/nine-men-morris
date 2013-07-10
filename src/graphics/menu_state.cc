// Copyright (c) 2013 Cristian Patrasciuc. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "graphics/menu_state.h"

#include <string>

#include "graphics/ogre_app.h"

#include "OGRE/OgreOverlayManager.h"

namespace graphics {

MenuState::MenuState(const std::string& menu_name)
    : GameState(&OgreApp::Instance()), menu_name_(menu_name) {}

bool MenuState::Initialize() {
  Ogre::Overlay* const overlay =
      Ogre::OverlayManager::getSingleton().getByName("TestMenu");
  overlay->show();
  return true;
}

}  // namespace graphics
