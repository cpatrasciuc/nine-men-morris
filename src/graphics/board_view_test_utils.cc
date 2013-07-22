// Copyright (c) 2013 Cristian Patrasciuc. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "graphics/board_view_test_utils.h"

#include <string>

#include "base/log.h"
#include "base/string_util.h"
#include "game/board_location.h"
#include "game/piece_color.h"
#include "graphics/board_view.h"
#include "graphics/ogre_app.h"

#include "OGRE/OgreCamera.h"
#include "OGRE/OgreEntity.h"
#include "OGRE/OgreSceneManager.h"
#include "OGRE/OgreSceneNode.h"
#include "OGRE/OgreVector2.h"
#include "OGRE/OgreVector3.h"

#include "OIS/OISMouse.h"

namespace graphics {
namespace {

Ogre::Vector2 ConvertWorldToScreen(const Ogre::Vector3& world_pos) {
  const Ogre::Camera* const camera = OgreApp::Instance().camera();
  const Ogre::Vector3 screen_pos =
      camera->getProjectionMatrix() * camera->getViewMatrix() * world_pos;
  return Ogre::Vector2(0.5f + 0.5f * screen_pos.x, 0.5f - 0.5f * screen_pos.y);
}

}  // anonymous namespace

const Ogre::Vector3& Get3DPosition(const BoardView& view,
                                   const game::BoardLocation& location) {
  return view.Get3DPosition(location);
}

void ClickOnLocation(BoardView* view, const game::BoardLocation& location) {
  const Ogre::Vector3 world_pos = Get3DPosition(*view, location);
  const Ogre::Vector2 screen_pos = ConvertWorldToScreen(world_pos);
  ClickAtScreenCoords(view, screen_pos.x, screen_pos.y);
}

// TODO(in_game_tests): This method is found in two other places. Refactor.
void ClickAtScreenCoords(BoardView* view, double screen_x, double screen_y) {
  DCHECK(0.0 <= screen_x && screen_x <= 1.0);
  DCHECK(0.0 <= screen_y && screen_y <= 1.0);
  const OIS::MouseState& real_mouse =
      OgreApp::Instance().mouse().getMouseState();
  OIS::MouseState state(real_mouse);
  state.X.abs = screen_x * state.width;
  state.Y.abs = screen_y * state.height;
  const OIS::MouseEvent move_event(NULL, state);
  view->mouseMoved(move_event);
  const OIS::MouseEvent press_event(NULL, state);
  view->mousePressed(press_event, OIS::MB_Left);
  const OIS::MouseEvent release_event(NULL, state);
  view->mouseReleased(release_event, OIS::MB_Left);
}

Ogre::SceneNode* GetPieceByColorAndPosition(game::PieceColor color,
                                            const Ogre::Vector3& position) {
  DCHECK(color != game::NO_COLOR);
  Ogre::SceneManager* const scene_mgr = OgreApp::Instance().scene_manager();
  const std::string prefix =
      color == game::WHITE_COLOR ? "WhitePiece" : "BlackPiece";
  int index = 0;
  while (true) {
    const std::string name = prefix + base::ToString(index);
    if (!scene_mgr->hasEntity(name)) {
      break;
    }
    Ogre::Entity* const entity = scene_mgr->getEntity(name);
    Ogre::SceneNode* const node = entity->getParentSceneNode();
    if (node->getPosition().positionEquals(position, 0.001)) {
      return node;
    }
    ++index;
  }
  return NULL;
}

}  // namespace graphics
