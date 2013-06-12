// Copyright (c) 2013 Cristian Patrasciuc. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef GRAPHICS_BOARD_RENDERER_H_
#define GRAPHICS_BOARD_RENDERER_H_

#include <map>

#include "base/basic_macros.h"
#include "graphics/graphics_export.h"

#include "OGRE/OgreTexture.h"

#include "OIS/OISMouse.h"

namespace game {
class BoardLocation;
class Game;
}

namespace Ogre {
class Entity;
class MovableObject;
class SceneNode;
}

namespace graphics {

class OgreApp;

class GRAPHICS_EXPORT BoardRenderer : public OIS::MouseListener {
 public:
  BoardRenderer(OgreApp* app, const game::Game& game_model);
  ~BoardRenderer();

  void Initialize();

  void EnableLocationSelection();
  void DisableLocationSelection();

  // MouseListener interface
  virtual bool mouseMoved(const OIS::MouseEvent& event);
  virtual bool mousePressed(const OIS::MouseEvent& event,
                            OIS::MouseButtonID id);
  virtual bool mouseReleased(const OIS::MouseEvent& event,
                             OIS::MouseButtonID id);

 private:
  void GenerateBoardTexture();

  OgreApp* app_;
  const game::Game& game_;
  std::map<Ogre::Entity*, game::BoardLocation> loc_map_;
  Ogre::MovableObject* selected_location_;
  bool location_selection_enabled_;

  DISALLOW_COPY_AND_ASSIGN(BoardRenderer);
};

}  // namespace graphics

#endif  // GRAPHICS_BOARD_RENDERER_H_

