// Copyright (c) 2013 Cristian Patrasciuc. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "graphics/menu_state.h"

#include <string>

#include "base/log.h"
#include "graphics/ogre_app.h"

#include "OGRE/OgreOverlay.h"
#include "OGRE/OgreOverlayElement.h"
#include "OGRE/OgreOverlayManager.h"

#include "OIS/OISKeyboard.h"
#include "OIS/OISMouse.h"

namespace graphics {
namespace {

// TODO(menu_state): Remove this once the OGRE bug is fixed.
// http://www.ogre3d.org/forums/viewtopic.php?f=2&t=63238
void FixOgreBugWithOverlays(Ogre::Overlay* overlay) {
  for (double x = 0.0; x <= 1.0; x += 0.05) {
    for (double y = 0.0; y <= 1.0; y += 0.05) {
      Ogre::OverlayElement* const element = overlay->findElementAt(x, y);
      if (element) {
        element->setCaption(element->getCaption());
      }
    }
  }
}

}  // anonymous namespace

MenuState::MenuState(const std::string& menu_name)
    : menu_name_(menu_name),
      menu_overlay_(NULL),
      reload_captions_(false) {}

bool MenuState::Initialize() {
  menu_overlay_ = Ogre::OverlayManager::getSingleton().getByName(menu_name_);
  DCHECK(menu_overlay_);
  menu_overlay_->show();
  reload_captions_ = true;
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

bool MenuState::keyReleased(const OIS::KeyEvent& event) {
  if (event.key == OIS::KC_ESCAPE && !escape_option_.empty()) {
    OnMenuOptionSelected(escape_option_);
  }
  return true;
}

bool MenuState::mouseReleased(const OIS::MouseEvent& event,
                              OIS::MouseButtonID id) {
  if (id != OIS::MB_Left) {
    return true;
  }
  const OIS::MouseState& state = event.state;
  const double screen_x = state.X.abs / static_cast<double>(state.width);
  const double screen_y = state.Y.abs / static_cast<double>(state.height);
  const Ogre::OverlayElement* const element =
      menu_overlay_->findElementAt(screen_x, screen_y);
  if (element && !element->getCaption().empty()) {
    OnMenuOptionSelected(element->getCaption());
  }
  return true;
}

bool MenuState::frameRenderingQueued(const Ogre::FrameEvent& event) {
  static bool was_called = false;
  if (was_called && reload_captions_) {
    FixOgreBugWithOverlays(menu_overlay_);
    reload_captions_ = false;
  }
  was_called = true;
  return true;
}

}  // namespace graphics
