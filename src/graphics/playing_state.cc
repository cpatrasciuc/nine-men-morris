// Copyright (c) 2013 Cristian Patrasciuc. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "graphics/playing_state.h"

#include "base/ptr/scoped_ptr.h"
#include "graphics/board_renderer.h"
#include "graphics/ogre_app.h"

#include "OGRE/OgreSceneManager.h"
#include "OGRE/OgreSceneNode.h"

namespace graphics {

PlayingState::PlayingState(OgreApp* app, game::GameOptions game_options)
    : GameState(app),
      game_(game_options),
      board_renderer_(new BoardRenderer(game_.board())) {}

bool PlayingState::Initialize() {
  board_renderer_->Initialize(app());
  return true;
}

void PlayingState::Exit() {
  Reset(board_renderer_);
  Ogre::SceneManager* const scene_manager = app()->scene_manager();
  Ogre::SceneNode* const root = scene_manager->getRootSceneNode();
  root->removeAllChildren();
}

bool PlayingState::keyPressed(const OIS::KeyEvent& event) {
  if (event.key == OIS::KC_ESCAPE) {
    app()->PopState();
  }
  return true;
}

}  // namespace graphics
