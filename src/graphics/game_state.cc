// Copyright (c) 2013 Cristian Patrasciuc. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "graphics/game_state.h"

#include "graphics/ogre_app.h"

namespace graphics {

GameState::GameState() {}

GameState::~GameState() {}

OgreApp* GameState::app() { return &OgreApp::Instance(); }

bool GameState::Initialize() { return true; }

void GameState::Exit() {}

void GameState::Pause() {}

void GameState::Resume() {}

bool GameState::keyPressed(const OIS::KeyEvent& event) { return false; }

bool GameState::keyReleased(const OIS::KeyEvent& event) { return false; }

bool GameState::mouseMoved(const OIS::MouseEvent& event) { return false; }

bool GameState::mousePressed(const OIS::MouseEvent& event,
                             OIS::MouseButtonID id) {
  return false;
}

bool GameState::mouseReleased(const OIS::MouseEvent& event,
                              OIS::MouseButtonID id) {
  return false;
}

}  // namespace graphics
