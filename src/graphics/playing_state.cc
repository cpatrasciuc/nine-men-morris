// Copyright (c) 2013 Cristian Patrasciuc. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "graphics/playing_state.h"

#include "OGRE/OgreCamera.h"
#include "OGRE/OgreEntity.h"
#include "OGRE/OgreMeshManager.h"
#include "OGRE/OgrePlane.h"
#include "OGRE/OgreSceneManager.h"
#include "OGRE/OgreSceneNode.h"

#include "graphics/ogre_app.h"

namespace graphics {

namespace {

const char kBoardEntityName[] = "BoardEntity";
const char kBoardPlaneName[] = "BoardPlane";

}  // anonymous namespace

PlayingState::PlayingState(OgreApp* app, game::GameOptions game_options)
    : GameState(app), game_(game_options) {}

bool PlayingState::Initialize() {
  Ogre::SceneManager* const scene_manager = app()->scene_manager();
  scene_manager->setAmbientLight(Ogre::ColourValue(0.2f, 0.2f, 0.2f));

  const int multiplier = 10;
  const int board_size = game_.board().size() * multiplier;
  Ogre::Camera* const camera = app()->camera();
  camera->setPosition(board_size, board_size, board_size);
  camera->lookAt(Ogre::Vector3::ZERO);

  Ogre::Plane board_plane(Ogre::Vector3::UNIT_Y, 0);
  Ogre::MeshManager::getSingleton().createPlane(kBoardPlaneName,
      Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME,
      board_plane, board_size, board_size, multiplier, multiplier,
      true, 1, 1, 1, Ogre::Vector3::UNIT_Z);
  Ogre::Entity* board_entity =
      scene_manager->createEntity(kBoardEntityName, kBoardPlaneName);
  Ogre::SceneNode* const root = scene_manager->getRootSceneNode();
  root->createChildSceneNode()->attachObject(board_entity);

  return true;
}

void PlayingState::Exit() {
  Ogre::SceneManager* const scene_manager = app()->scene_manager();
  Ogre::SceneNode* const root = scene_manager->getRootSceneNode();
  root->removeAllChildren();
  Ogre::MeshManager::getSingleton().remove(kBoardPlaneName);
}

bool PlayingState::keyPressed(const OIS::KeyEvent& event) {
  if (event.key == OIS::KC_ESCAPE) {
    app()->PopState();
  }
  return true;
}

}  // namespace graphics
