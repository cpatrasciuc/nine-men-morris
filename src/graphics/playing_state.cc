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
      board_renderer_(new BoardRenderer(game_.board())),
      camera_controller_() {}

bool PlayingState::Initialize() {
  board_renderer_->Initialize(app());
  camera_controller_.set_min_distance(50);
  camera_controller_.set_max_distance(200);
  camera_controller_.set_camera(app()->camera());
  return true;
}

void PlayingState::Exit() {
  camera_controller_.set_camera(NULL);
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

bool PlayingState::mouseMoved(const OIS::MouseEvent& event) {
  camera_controller_.mouseMoved(event);
  board_renderer_->mouseMoved(event);
  return true;
}

bool PlayingState::mousePressed(const OIS::MouseEvent& event,
                                OIS::MouseButtonID id) {
  camera_controller_.mousePressed(event, id);
  return true;
}
bool PlayingState::mouseReleased(const OIS::MouseEvent& event,
                                 OIS::MouseButtonID id) {
  camera_controller_.mouseReleased(event, id);
  return true;
}

}  // namespace graphics
