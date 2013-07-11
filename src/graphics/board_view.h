// Copyright (c) 2013 Cristian Patrasciuc. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef GRAPHICS_BOARD_VIEW_H_
#define GRAPHICS_BOARD_VIEW_H_

#include <deque>
#include <map>
#include <vector>

#include "base/basic_macros.h"
#include "base/supports_listener.h"
#include "game/game_listener.h"
#include "game/piece_color.h"
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
    REMOVABLE_BLACK_PIECE = (1 << 4),
    CUSTOM = (1 << 5)
  };

  BoardView(OgreApp* app, const game::Game& game_model);
  ~BoardView();

  const game::Game& game_model() const { return game_; }

  void Initialize();

  void SetSelectionType(unsigned int selection_type);
  void SetCustomSelectableLocations(
      const std::vector<game::BoardLocation>& selectable);

  // MouseListener interface
  virtual bool mouseMoved(const OIS::MouseEvent& event);
  virtual bool mousePressed(const OIS::MouseEvent& event,
                            OIS::MouseButtonID id);
  virtual bool mouseReleased(const OIS::MouseEvent& event,
                             OIS::MouseButtonID id);

 private:
  typedef std::map<game::BoardLocation, int> IndexMap;

  void GenerateBoardTexture();
  void InitializePieces(Ogre::SceneNode* board_view_root);

  void FireOnLocationSelected(const game::BoardLocation& location);

  void ClearSelection();
  void UpdateRemovablePieces(game::PieceColor color);

  // game::GameListener overrides
  virtual void OnPlayerAction(const game::PlayerAction& action);

  Ogre::SceneNode* GetPieceByColorAndIndex(game::PieceColor color,
                                               int index);
  Ogre::SceneNode* GetPieceByLocation(const game::BoardLocation& location);

  Ogre::MovableObject* GetLocationSelector(const game::BoardLocation& loc);
  const Ogre::Vector3& Get3DPosition(const game::BoardLocation& location) const;

  IndexMap* GetIndexMapByColor(game::PieceColor color);

  void MovePiece(const game::BoardLocation& from,
                 const game::BoardLocation& to,
                 game::PieceColor color);
  void AddPiece(const game::BoardLocation& to, game::PieceColor color);
  void RemovePiece(const game::BoardLocation& from, game::PieceColor color);

  void UpdateSelection(const OIS::MouseState& state);

  OgreApp* app_;
  const game::Game& game_;

  std::map<Ogre::MovableObject*, game::BoardLocation> locations_;
  std::map<game::BoardLocation, Ogre::MovableObject*> reverse_loc_map_;
  std::map<game::BoardLocation, const Ogre::Vector3*> positions_;

  IndexMap white_index_map_;
  IndexMap black_index_map_;

  int white_place_index_;
  int black_place_index_;

  Ogre::SceneNode* white_pieces_;
  Ogre::SceneNode* black_pieces_;

  Ogre::MovableObject* temp_selected_location_;
  Ogre::MovableObject* selected_location_;
  unsigned int selection_type_;

  friend void ClickOnLocation(BoardView*, const game::BoardLocation&);
  DISALLOW_COPY_AND_ASSIGN(BoardView);
};

}  // namespace graphics

#endif  // GRAPHICS_BOARD_VIEW_H_

