// Copyright (c) 2013 Cristian Patrasciuc. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef GRAPHICS_BOARD_VIEW_H_
#define GRAPHICS_BOARD_VIEW_H_

#include <deque>
#include <map>

#include "base/basic_macros.h"
#include "base/supports_listener.h"
#include "game/game_listener.h"
#include "graphics/graphics_export.h"

#include "OGRE/OgreTexture.h"

#include "OIS/OISMouse.h"

namespace game {
class BoardLocation;
class Game;
class PlayerAction;
}

namespace Ogre {
class MovableObject;
class SceneNode;
class Vector3;
}

namespace graphics {

class OgreApp;
class SelectionListener;

class GRAPHICS_EXPORT BoardView
    : public OIS::MouseListener,
      public game::GameListener,
      public base::SupportsListener<SelectionListener> {
 public:
  enum SelectionType {
    NONE = 0,
    EMPTY_LOCATION = 1,
    ANY_WHITE_PIECE = (1 << 1),
    ANY_BLACK_PIECE = (1 << 2),
    REMOVABLE_WHITE_PIECE = (1 << 3),
    REMOVABLE_BLACK_PIECE = (1 << 4)
  };

  BoardView(OgreApp* app, const game::Game& game_model);
  ~BoardView();

  void Initialize();

  void SetSelectionType(unsigned int selection_type);

  // MouseListener interface
  virtual bool mouseMoved(const OIS::MouseEvent& event);
  virtual bool mousePressed(const OIS::MouseEvent& event,
                            OIS::MouseButtonID id);
  virtual bool mouseReleased(const OIS::MouseEvent& event,
                             OIS::MouseButtonID id);

 private:
  void GenerateBoardTexture();
  void InitializePieces();

  void FireOnLocationSelected(const game::BoardLocation& location);
  void FireOnSelectionCleared();
  void ClearSelection();

  // game::GameListener overrides
  virtual void OnPlayerAction(const game::PlayerAction& action);

  OgreApp* app_;
  const game::Game& game_;

  std::map<Ogre::MovableObject*, game::BoardLocation> loc_map_;
  std::map<game::BoardLocation, Ogre::SceneNode*> pieces_;
  std::map<game::BoardLocation, const Ogre::Vector3*> positions_;

  std::map<game::BoardLocation, int> white_pieces_;
  std::map<game::BoardLocation, int> black_pieces_;

  int white_place_index_;
  int black_place_index_;

  Ogre::SceneNode* white_node_;
  Ogre::SceneNode* black_node_;

  Ogre::MovableObject* temp_selected_location_;
  Ogre::MovableObject* selected_location_;
  unsigned int selection_type_;

  DISALLOW_COPY_AND_ASSIGN(BoardView);
};

}  // namespace graphics

#endif  // GRAPHICS_BOARD_VIEW_H_

