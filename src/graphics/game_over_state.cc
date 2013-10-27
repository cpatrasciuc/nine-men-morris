// Copyright (c) 2013 Cristian Patrasciuc. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "graphics/game_over_state.h"

#include <sstream>
#include <string>

#include "base/log.h"
#include "game/piece_color.h"
#include "graphics/ogre_app.h"

#include "OGRE/OgreOverlayElement.h"
#include "OGRE/OgreOverlayManager.h"

namespace graphics {

const char kOKOption[] = "OK";

GameOverState::GameOverState(game::PieceColor winner)
    : MenuState("GameOver"), winner_(winner) {
  DCHECK(winner_ != game::NO_COLOR);
}

bool GameOverState::Initialize() {
  if (!MenuState::Initialize()) {
    return false;
  }
  std::ostringstream message;
  message << "GAME OVER!" << std::endl;
  message << (winner_ == game::WHITE_COLOR ? "White" : "Black");
  message << " wins!";
  Ogre::OverlayElement* const game_over_text_area =
      Ogre::OverlayManager::getSingleton().getOverlayElement("GameOverText");
  game_over_text_area->setCaption(message.str());
  set_escape_option(kOKOption);
  return true;
}

void GameOverState::OnMenuOptionSelected(const std::string& option) {
  if (option == kOKOption) {
    app()->PopState();
  }
}

}  // namespace graphics
