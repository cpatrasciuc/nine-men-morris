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
class Board;
class BoardLocation;
}

namespace Ogre {
class Entity;
class MovableObject;
}

namespace graphics {

class OgreApp;

class GRAPHICS_EXPORT BoardRenderer : public OIS::MouseListener {
 public:
  BoardRenderer(OgreApp* app, const game::Board& board);
  ~BoardRenderer();

  void Initialize();

  // MouseListener interface
  virtual bool mouseMoved(const OIS::MouseEvent& event);
  virtual bool mousePressed(const OIS::MouseEvent& event,
                            OIS::MouseButtonID id);
  virtual bool mouseReleased(const OIS::MouseEvent& event,
                             OIS::MouseButtonID id);

 private:
  void GenerateBoardTexture();

  OgreApp* app_;
  const game::Board& board_;
  std::map<Ogre::Entity*, game::BoardLocation> loc_map_;
  Ogre::MovableObject* selected_location_;

  DISALLOW_COPY_AND_ASSIGN(BoardRenderer);
};

}  // namespace graphics

#endif  // GRAPHICS_BOARD_RENDERER_H_

