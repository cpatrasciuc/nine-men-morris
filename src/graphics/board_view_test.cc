// Copyright (c) 2013 Cristian Patrasciuc. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include <memory>
#include <vector>

#include "base/bind.h"
#include "base/location.h"
#include "base/method.h"
#include "base/ptr/scoped_ptr.h"
#include "game/board.h"
#include "game/board_location.h"
#include "game/game.h"
#include "game/game_test_helper.h"
#include "game/piece_color.h"
#include "graphics/board_view.h"
#include "graphics/board_view_test_utils.h"
#include "graphics/in_game_test.h"
#include "graphics/ogre_app.h"
#include "graphics/selection_listener.h"
#include "gtest/gtest.h"

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

  virtual void TestMethod() {
    // Add a few frames delay to allow the BoardView to fully initialize.
    // TODO(in_game_tests): Add a generic mechanism to add delay frames.
    static int frame_counter = 0;
    const int delay_count = 10;
    if (frame_counter == 0) {
      InitializeBoardView();
    }
    if (frame_counter < delay_count) {
      ++frame_counter;
      // Repost TestMethod() on the game loop so it will be called during the
      // next frame.
      typedef void(BoardViewTestBase::*TestMethodSig)(void);
      OgreApp::Instance().PostTaskOnGameLoop(FROM_HERE, base::Bind(
          new base::Method<TestMethodSig>(&BoardViewTestBase::TestMethod),
          this));
    } else {
      DelayedTestMethod();
      SUCCEED();
    }
  }

 protected:
  BoardViewTestBase() {}

  virtual void InitializeBoardView() = 0;
  virtual void DelayedTestMethod() = 0;
};

class BoardViewSelectionTest : public BoardViewTestBase,
                               public SelectionListener {
 public:
  BoardViewSelectionTest()
      : event_was_fired_(false), expected_location_(-1, -1) {}

  virtual void Done() {
    Reset(view_);
    BoardViewTestBase::Done();
  }

 private:
  virtual void InitializeBoardView() {
    // TODO(game): Add default argument for game::Game constructor.
    game_ = game::LoadSavedGameForTests("remove_from_mill_6");
    Reset(view_, new BoardView(*game_));
    view_->Initialize();
  }

  virtual void DelayedTestMethod() {
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
    ClickAtScreenCoords(Get(view_), 0.001, 0.001);
    EXPECT_FALSE(event_was_fired_);

    view_->RemoveListener(this);
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

  std::auto_ptr<game::Game> game_;
  base::ptr::scoped_ptr<BoardView> view_;
  bool event_was_fired_;
  game::BoardLocation expected_location_;
};

IN_GAME_TEST(BoardViewSelectionTest, Selection);

}  // anonymous namespace
}  // namespace graphics
