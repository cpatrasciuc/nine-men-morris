// Copyright (c) 2013 Cristian Patrasciuc. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include <memory>
#include <string>
#include <vector>

#include "base/log.h"
#include "base/ptr/scoped_ptr.h"
#include "game/board.h"
#include "game/board_location.h"
#include "game/game.h"
#include "game/game_test_helper.h"
#include "game/piece_color.h"
#include "game/player_action.h"
#include "graphics/board_view.h"
#include "graphics/board_view_test_utils.h"
#include "graphics/in_game_test.h"
#include "graphics/ogre_app.h"
#include "graphics/ois_test_utils.h"
#include "graphics/selection_listener.h"
#include "gtest/gtest.h"

#include "OGRE/OgreMovableObject.h"
#include "OGRE/OgreSceneManager.h"
#include "OGRE/OgreSceneNode.h"
#include "OGRE/OgreVector3.h"

namespace graphics {
namespace {

typedef bool (*LocationPred)(const game::Board&, const game::BoardLocation&);
bool IsWhite(const game::Board& board, const game::BoardLocation& loc) {
  return board.GetPieceAt(loc) == game::WHITE_COLOR;
}
bool IsBlack(const game::Board& board, const game::BoardLocation& loc) {
  return board.GetPieceAt(loc) == game::BLACK_COLOR;
}
bool IsEmpty(const game::Board& board, const game::BoardLocation& loc) {
  return board.GetPieceAt(loc) == game::NO_COLOR;
}
bool IsNonEmpty(const game::Board& board, const game::BoardLocation& loc) {
  return !IsEmpty(board, loc);
}
bool IsWhiteAndRemovable(const game::Board& board,
                         const game::BoardLocation& loc) {
  return IsWhite(board, loc) && !board.IsPartOfMill(loc);
}
bool CustomPredicate(const game::Board& board, const game::BoardLocation& loc) {
  return loc.line() == 0 && loc.column() == 0;
}

class BoardViewTestBase : public InGameTestBase {
 public:
  virtual ~BoardViewTestBase() {}

  virtual bool Initialize() {
    InitializeGameModel();
    Reset(view_, new BoardView(*game_));
    view_->Initialize();
    return true;
  }

  virtual void Done() {
    Reset(view_);
    InGameTestBase::Done();
  }

 protected:
  BoardViewTestBase() {}

  virtual void InitializeGameModel() = 0;

  // TODO(board_view_test): Make these private and add accessors?
  std::auto_ptr<game::Game> game_;
  base::ptr::scoped_ptr<BoardView> view_;

 private:
};

// TODO(board_view_test): Add a test with animations enabled?

class BoardViewSelectionTest : public BoardViewTestBase,
                               public SelectionListener {
 public:
  BoardViewSelectionTest()
      : event_was_fired_(false), expected_location_(-1, -1) {}

 private:
  virtual void InitializeGameModel() {
    game_ = game::LoadSavedGameForTests("remove_from_mill_6");
  }

  virtual void TestMethod() {
    view_->AddListener(this);

    view_->SetSelectionType(BoardView::EMPTY_LOCATION);
    AssertSelectableLocations(&IsEmpty);

    view_->SetSelectionType(BoardView::ANY_WHITE_PIECE);
    AssertSelectableLocations(&IsWhite);

    view_->SetSelectionType(
        BoardView::ANY_WHITE_PIECE | BoardView::ANY_BLACK_PIECE);
    AssertSelectableLocations(&IsNonEmpty);

    view_->SetSelectionType(BoardView::ANY_BLACK_PIECE);
    AssertSelectableLocations(&IsBlack);

    view_->SetSelectionType(BoardView::REMOVABLE_WHITE_PIECE);
    AssertSelectableLocations(&IsWhiteAndRemovable);

    std::vector<game::BoardLocation> selectable;
    selectable.push_back(game::BoardLocation(0, 0));
    view_->SetCustomSelectableLocations(selectable);
    view_->SetSelectionType(BoardView::CUSTOM);
    AssertSelectableLocations(&CustomPredicate);

    // Click on an empty place. It shouldn't trigger any selection event.
    event_was_fired_ = false;
    expected_location_ = game::BoardLocation(-1, -1);
    SimulateClick(Get(view_), 0.001, 0.001);
    EXPECT_FALSE(event_was_fired_);

    view_->RemoveListener(this);
    SUCCEED();
  }

  void AssertSelectableLocations(LocationPred predicate) {
    const game::Board& board = view_->game_model().board();
    const std::vector<game::BoardLocation>& locations = board.locations();
    for (size_t i = 0; i < locations.size(); ++i) {
      event_was_fired_ = false;
      expected_location_ = locations[i];
      ClickOnLocation(Get(view_), expected_location_);
      EXPECT_EQ((*predicate)(board, locations[i]), event_was_fired_);
    }
  }

  virtual void OnLocationSelected(const game::BoardLocation& location) {
    event_was_fired_ = true;
    EXPECT_EQ(expected_location_, location);
  }

  bool event_was_fired_;
  game::BoardLocation expected_location_;
};

IN_GAME_TEST(BoardViewSelectionTest, Selection);

class BoardViewPlacePieceTest : public BoardViewTestBase {
 private:
  virtual void InitializeGameModel() {
    game_.reset(new game::Game());
    game_->Initialize();
  }

  virtual void TestMethod() {
    game_->AddListener(Get(view_));
    const game::BoardLocation location(0, 0);
    const Ogre::Vector3 position = Get3DPosition(*view_, location);
    const game::PieceColor color = view_->game_model().current_player();
    Ogre::SceneNode* const piece_node = GetPieceByColorAndIndex(color, 0);
    const Ogre::MovableObject* const piece_entity =
        piece_node->getAttachedObject(0);
    EXPECT_TRUE(piece_entity->isVisible());
    game::PlayerAction action(color, game::PlayerAction::PLACE_PIECE);
    action.set_destination(location);
    game_->ExecutePlayerAction(action);
    EXPECT_TRUE(piece_entity->isVisible());
    EXPECT_TRUE(position.positionEquals(piece_node->getPosition(), 0.001));
    game_->RemoveListener(Get(view_));
    SUCCEED();
  }

  Ogre::SceneNode* GetPieceByColorAndIndex(game::PieceColor color, int index) {
    DCHECK(color != game::NO_COLOR);
    Ogre::SceneManager* const scene_mgr = OgreApp::Instance().scene_manager();
    const std::string node_name =
        color == game::WHITE_COLOR ? "AllWhitePieces" : "AllBlackPieces";
    Ogre::SceneNode* const all_pieces_node = scene_mgr->getSceneNode(node_name);
    if (all_pieces_node->numChildren() > 0) {
      return static_cast<Ogre::SceneNode*>(all_pieces_node->getChild(0));
    }
    return NULL;
  }
};

IN_GAME_TEST(BoardViewPlacePieceTest, Place);

class BoardViewMovePieceTest : public BoardViewTestBase {
 private:
  virtual void InitializeGameModel() {
    game_ = game::LoadSavedGameForTests("place_phase_3");
  }

  virtual void TestMethod() {
    game_->AddListener(Get(view_));
    const game::BoardLocation source(0, 0);
    const game::BoardLocation destination(1, 0);
    const Ogre::Vector3 source_pos = Get3DPosition(*view_, source);
    const Ogre::Vector3 dest_pos = Get3DPosition(*view_, destination);
    const game::PieceColor color = view_->game_model().current_player();
    Ogre::SceneNode* const piece_node =
        GetPieceByColorAndPosition(color, source_pos);
    ASSERT_TRUE(piece_node);
    const Ogre::MovableObject* const piece_entity =
        piece_node->getAttachedObject(0);
    EXPECT_TRUE(piece_entity->isVisible());
    EXPECT_FALSE(GetPieceByColorAndPosition(color, dest_pos));
    game::PlayerAction action(color, game::PlayerAction::MOVE_PIECE);
    action.set_source(source);
    action.set_destination(destination);
    game_->ExecutePlayerAction(action);
    EXPECT_TRUE(piece_entity->isVisible());
    EXPECT_TRUE(dest_pos.positionEquals(piece_node->getPosition(), 0.001));
    EXPECT_FALSE(GetPieceByColorAndPosition(color, source_pos));
    game_->RemoveListener(Get(view_));
    SUCCEED();
  }
};

IN_GAME_TEST(BoardViewMovePieceTest, Move);

class BoardViewRemovePieceTest : public BoardViewTestBase {
 private:
  virtual void InitializeGameModel() {
    game_ = game::LoadSavedGameForTests("remove_from_mill_6");
  }

  virtual void TestMethod() {
    game_->AddListener(Get(view_));
    const game::BoardLocation source(0, 0);
    const Ogre::Vector3 source_pos = Get3DPosition(*view_, source);
    const game::PieceColor color = view_->game_model().current_player();
    Ogre::SceneNode* const piece_node =
        GetPieceByColorAndPosition(game::GetOpponent(color), source_pos);
    ASSERT_TRUE(piece_node);
    const Ogre::MovableObject* const piece_entity =
        piece_node->getAttachedObject(0);
    EXPECT_TRUE(piece_entity->isVisible());
    game::PlayerAction action(color, game::PlayerAction::REMOVE_PIECE);
    action.set_source(source);
    game_->ExecutePlayerAction(action);
    EXPECT_FALSE(piece_entity->isVisible());
    game_->RemoveListener(Get(view_));
    SUCCEED();
  }
};

IN_GAME_TEST(BoardViewRemovePieceTest, Remove);

// This test validates that upon deletion the BoardView class completely removes
// all OGRE entities, lights, etc. so that a new view can be instantiated later
// on without having name conflicts inside the OGRE scene.
class BoardViewMultipleInstances : public InGameTestBase {
 private:
  virtual void TestMethod() {
    game::Game game_model;
    {
      base::ptr::scoped_ptr<BoardView> first_view_(new BoardView(game_model));
      first_view_->Initialize();
    }
    {
      base::ptr::scoped_ptr<BoardView> second_view_(new BoardView(game_model));
      second_view_->Initialize();
    }
    SUCCEED();
  }
};

IN_GAME_TEST(BoardViewMultipleInstances, BoardViewMultipleInstances);

}  // anonymous namespace
}  // namespace graphics
