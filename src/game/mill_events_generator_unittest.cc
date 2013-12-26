// Copyright (c) 2013 Cristian Patrasciuc. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include <memory>
#include <set>
#include <vector>

#include "base/log.h"
#include "game/game.h"
#include "game/game_test_helper.h"
#include "game/mill_event_listener.h"
#include "game/mill_events_generator.h"
#include "game/player_action.h"
#include "gtest/gtest.h"

namespace game {
namespace {

class TestMillEventListener : public MillEventListener {
 public:
  virtual void OnMillEvent(const BoardLocation& location, bool mill) {
    LOG(DEBUG) << location.line() << " " << location.column() << " " << mill;
    if (mill) {
      mills_.insert(location);
    } else {
      EXPECT_TRUE(mills_.count(location));
      mills_.erase(location);
    }
  }

  void AssertMillPieceCount(int expected_pieces) {
    EXPECT_EQ(expected_pieces, static_cast<int>(mills_.size()));
  }

 private:
  std::set<BoardLocation> mills_;
};

TEST(MillEventsGenerator, Basic) {
  std::auto_ptr<Game> saved_game = LoadSavedGameForTests("remove_from_mill_6");
  std::vector<PlayerAction> actions;
  saved_game->DumpActionList(&actions);

  Game test_game(saved_game->options());
  test_game.Initialize();
  MillEventsGenerator generator(&test_game);
  TestMillEventListener listener;
  generator.AddListener(&listener);

  for (size_t i = 0; i < actions.size(); ++i) {
    test_game.ExecutePlayerAction(actions[i]);
  }
  listener.AssertMillPieceCount(6);

  for (size_t i = 0; i < actions.size(); ++i) {
    test_game.UndoLastAction();
  }
  listener.AssertMillPieceCount(0);

  generator.RemoveListener(&listener);
}

}  // anonymous namespace
}  // namespace game
