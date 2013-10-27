// Copyright (c) 2013 Cristian Patrasciuc. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "graphics/confirmation_menu_state.h"

#include <string>

#include "graphics/menu_state.h"
#include "graphics/ogre_app.h"

#include "OGRE/OgreOverlayElement.h"
#include "OGRE/OgreOverlayManager.h"

namespace graphics {
namespace {

const char kConfirmationMenuName[] = "ConfirmationMenu";
const char kYesOption[] = "Yes";
const char kNoOption[] = "No";

}  // anonymous namespace

ConfirmationMenuState::ConfirmationMenuState(const std::string& question)
    : MenuState(kConfirmationMenuName), question_(question), confirmed_(false) {
}

bool ConfirmationMenuState::Initialize() {
  if (!MenuState::Initialize()) {
    return false;
  }
  Ogre::OverlayElement* const question_text_area =
      Ogre::OverlayManager::getSingleton().getOverlayElement("QuestionText");
  question_text_area->setCaption(question_);
  set_escape_option(kNoOption);
  return true;
}

void ConfirmationMenuState::OnMenuOptionSelected(const std::string& option) {
  if (option == kYesOption) {
    confirmed_ = true;
  }
  if (option == kYesOption || option == kNoOption) {
    OgreApp::Instance().PopState();
  }
}

}  // namespace graphics
