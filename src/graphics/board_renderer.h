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
  enum SelectionType {
    NONE = 0,
    EMPTY_LOCATION = 1,
    ANY_WHITE_PIECE = (1 << 1),
    ANY_BLACK_PIECE = (1 << 2),
    REMOVABLE_WHITE_PIECE = (1 << 3),
    REMOVABLE_BLACK_PIECE = (1 << 4)
  };


  BoardRenderer(OgreApp* app, const game::Game& game_model);
  ~BoardRenderer();

  void Initialize();

  void SetSelectionType(const SelectionType& selection_type);

  // MouseListener interface
  virtual bool mouseMoved(const OIS::MouseEvent& event);
  virtual bool mousePressed(const OIS::MouseEvent& event,
                            OIS::MouseButtonID id);
  virtual bool mouseReleased(const OIS::MouseEvent& event,
                             OIS::MouseButtonID id);

 private:
  void GenerateBoardTexture();
  void InitializePieces();

  OgreApp* app_;
  const game::Game& game_;
  std::map<Ogre::Entity*, game::BoardLocation> loc_map_;
  Ogre::MovableObject* selected_location_;
  SelectionType selection_type_;

  DISALLOW_COPY_AND_ASSIGN(BoardRenderer);
};

}  // namespace graphics

#endif  // GRAPHICS_BOARD_RENDERER_H_

