// Copyright (c) 2013 Cristian Patrasciuc. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "game/mill_events_generator.h"

#include <algorithm>
#include <queue>
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
  std::queue<BoardLocation> q;
  switch (action.type()) {
    case PlayerAction::MOVE_PIECE:
      q.push(action.source());
      q.push(action.destination());
      break;
    case PlayerAction::PLACE_PIECE:
      q.push(action.destination());
      break;
    case PlayerAction::REMOVE_PIECE:
      q.push(action.source());
      break;
  }

  while (!q.empty()) {
    const BoardLocation loc = q.front();
    q.pop();

    std::vector<BoardLocation>::iterator it =
        std::lower_bound(mills_.begin(), mills_.end(), loc);
    const bool was_part_of_mill = (it != mills_.end() && *it == loc);
    const bool is_part_of_mill = game_->board().IsPartOfMill(loc);

    if (is_part_of_mill != was_part_of_mill) {
      FireMillEvent(loc, is_part_of_mill);
      if (is_part_of_mill) {
        mills_.insert(it, loc);
      } else {
        mills_.erase(it);
      }
      std::vector<BoardLocation> adjacent_locations;
      game_->board().GetAdjacentLocations(loc, &adjacent_locations);
      for (size_t i = 0; i < adjacent_locations.size(); ++i) {
        q.push(adjacent_locations[i]);
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
