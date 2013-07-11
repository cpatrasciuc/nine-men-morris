// Copyright (c) 2013 Cristian Patrasciuc. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "graphics/menu_state.h"

#include <string>

#include "base/log.h"
#include "graphics/ogre_app.h"

#include "OGRE/OgreOverlayManager.h"

namespace graphics {

MenuState::MenuState(const std::string& menu_name)
    : GameState(&OgreApp::Instance()),
      menu_name_(menu_name),
      menu_overlay_(NULL) {}

bool MenuState::Initialize() {
  // TODO(menu_state): Check if the overlay should be destroyed.
  menu_overlay_ = Ogre::OverlayManager::getSingleton().getByName(menu_name_);
  DCHECK(menu_overlay_);
  menu_overlay_->show();
  return true;
}

void MenuState::Exit() {
  DCHECK(menu_overlay_);
  menu_overlay_->hide();
}

void MenuState::Pause() {
  DCHECK(menu_overlay_);
  menu_overlay_->hide();
}

void MenuState::Resume() {
  DCHECK(menu_overlay_);
  menu_overlay_->show();
}

}  // namespace graphics
