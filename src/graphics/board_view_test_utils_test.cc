// Copyright (c) 2013 Cristian Patrasciuc. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include <vector>

#include "base/bind.h"
#include "base/location.h"
#include "base/method.h"
#include "base/ptr/scoped_ptr.h"
#include "game/board.h"
#include "game/board_location.h"
#include "game/game.h"
#include "game/game_options.h"
#include "graphics/board_view.h"
#include "graphics/board_view_test_utils.h"
#include "graphics/in_game_test.h"
#include "graphics/ogre_app.h"
#include "graphics/selection_listener.h"
#include "gtest/gtest.h"

namespace graphics {
namespace {

class BoardViewTestUtilsTest : public InGameTestBase,
                               public SelectionListener {
 public:
  BoardViewTestUtilsTest()
      : event_was_fired_(false), expected_location_(-1, -1) {}

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
      typedef void(BoardViewTestUtilsTest::*TestMethodSig)(void);
      OgreApp::Instance().PostTaskOnGameLoop(FROM_HERE, base::Bind(
          new base::Method<TestMethodSig>(&BoardViewTestUtilsTest::TestMethod),
          this));
    } else {
      DelayedTestMethod();
      SUCCEED();
    }
  }

  virtual void Done() {
    Reset(view_);
    InGameTestBase::Done();
  }

 private:
  void InitializeBoardView() {
    // TODO(game): Add default argument for game::Game constructor.
    const game::GameOptions options;
    const game::Game* const game_model = new game::Game(options);
    Reset(view_, new BoardView(&OgreApp::Instance(), *game_model));
    view_->Initialize();
  }

  void DelayedTestMethod() {
    view_->AddListener(this);

    view_->SetSelectionType(BoardView::EMPTY_LOCATION);
    const std::vector<game::BoardLocation>& locations
        = view_->game_model().board().locations();
    for (size_t i = 0; i < locations.size(); ++i) {
      event_was_fired_ = false;
      expected_location_ = locations[i];
      ClickOnLocation(Get(view_), expected_location_);
      EXPECT_TRUE(event_was_fired_);
    }

    std::vector<game::BoardLocation> selectable;
    selectable.push_back(game::BoardLocation(0, 0));
    view_->SetCustomSelectableLocations(selectable);
    view_->SetSelectionType(BoardView::CUSTOM);
    for (size_t i = 0; i < locations.size(); ++i) {
      event_was_fired_ = false;
      expected_location_ = locations[i];
      ClickOnLocation(Get(view_), expected_location_);
      EXPECT_EQ(expected_location_ == selectable[0], event_was_fired_);
    }

    // Click on an empty place. It shouldn't trigger any selection event.
    event_was_fired_ = false;
    expected_location_ = game::BoardLocation(-1, -1);
    ClickAtScreenCoords(Get(view_), 0.001, 0.001);
    EXPECT_FALSE(event_was_fired_);

    view_->RemoveListener(this);
  }

  virtual void OnLocationSelected(const game::BoardLocation& location) {
    event_was_fired_ = true;
    EXPECT_EQ(expected_location_, location);
  }

  base::ptr::scoped_ptr<BoardView> view_;
  bool event_was_fired_;
  game::BoardLocation expected_location_;
};

IN_GAME_TEST(BoardViewTestUtilsTest, ClickOnLocation);

}  // anonymous namespace
}  // namespace graphics
