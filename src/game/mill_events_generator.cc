// Copyright (c) 2013 Cristian Patrasciuc. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "game/mill_events_generator.h"

#include <vector>

#include "game/board_location.h"
#include "game/game.h"
#include "game/mill_event_listener.h"
#include "game/player_action.h"

namespace game {

MillEventsGenerator::MillEventsGenerator(Game* game_model)
    : game_(game_model) {
  game_->AddListener(this);
}

MillEventsGenerator::~MillEventsGenerator() {
  game_->RemoveListener(this);
}

void MillEventsGenerator::OnPlayerAction(const PlayerAction& action) {
  // TODO(mill_effect_generator): Can find a better algorithm instead of
  // iterating through all board locations?
  const std::vector<BoardLocation>& locations = game_->board().locations();
  for (size_t i = 0; i < locations.size(); ++i) {
    const std::vector<BoardLocation>::iterator it =
        std::lower_bound(mills_.begin(), mills_.end(), locations[i]);
    const bool was_part_of_mill = (it != mills_.end() && *it == locations[i]);
    const bool is_part_of_mill = game_->board().IsPartOfMill(locations[i]);

    if (is_part_of_mill != was_part_of_mill) {
      FireMillEvent(locations[i], is_part_of_mill);
      if (is_part_of_mill) {
        mills_.insert(it, locations[i]);
      } else {
        mills_.erase(it);
      }
    }
  }
}

void MillEventsGenerator::OnUndoPlayerAction(const PlayerAction& action) {
  OnPlayerAction(action);
}

void MillEventsGenerator::FireMillEvent(const BoardLocation& loc, bool mill) {
  const ListenerList list(listeners());
  for (ListenerIter it = list.begin(); it != list.end(); ++it) {
    (*it)->OnMillEvent(loc, mill);
  }
}

}  // namespace game
