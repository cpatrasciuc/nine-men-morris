// Copyright (c) 2013 Cristian Patrasciuc. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef GAME_MILL_EVENTS_GENERATOR_H_
#define GAME_MILL_EVENTS_GENERATOR_H_

#include <vector>

#include "base/supports_listener.h"
#include "game/board_location.h"
#include "game/game_export.h"
#include "game/game_listener.h"
#include "game/mill_event_listener.h"

namespace game {

class Game;
class PlayerAction;

class GAME_EXPORT MillEventsGenerator
    : public base::SupportsListener<MillEventListener>,
      public GameListener {
 public:
  explicit MillEventsGenerator(Game* game_model);
  virtual ~MillEventsGenerator();

 private:
  // GameListener overides
  virtual void OnPlayerAction(const PlayerAction& action);
  virtual void OnUndoPlayerAction(const PlayerAction& action);

  // Sends an event to all MillEventListeners that the given board |location|
  // has changed its mill-status. The new status is given by the |mill|
  // argument.
  void FireMillEvent(const BoardLocation& location, bool mill);

  Game* game_;

  // Holds all board locations that are part of mills.
  std::vector<BoardLocation> mills_;
};

}  // namespace game

#endif  // GAME_MILL_EVENTS_GENERATOR_H_

