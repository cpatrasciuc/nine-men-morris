// Copyright (c) 2012 Cristian Patrasciuc. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "game/game_serializer.h"

#include <stdint.h>

#include <limits>
#include <ostream>
#include <vector>

#include "base/log.h"
#include "game/board.h"
#include "game/game.h"
#include "game/player_action.h"

namespace game {

namespace {

void SerializeActionsToBinaryStream(const std::vector<PlayerAction>& actions,
                                    std::ostream& out) {
  const int64_t count = actions.size();
  out.write(reinterpret_cast<const char*>(&count), sizeof(count));
  for (size_t i = 0; i < actions.size(); ++i) {
    const char type = static_cast<const char>(actions[i].type());
    out.write(&type, sizeof(type));
    const char color = static_cast<const char>(actions[i].player_color());
    out.write(&color, sizeof(color));
    const char action_info[] = {
        actions[i].source().line(),
        actions[i].source().column(),
        actions[i].destination().line(),
        actions[i].destination().column()
    };
    switch (actions[i].type()) {
      case PlayerAction::MOVE_PIECE:
        out.write(action_info, 4 * sizeof(action_info[0]));
        break;
      case PlayerAction::PLACE_PIECE:
        out.write(action_info + 2, 2 * sizeof(action_info[0]));
        break;
      case PlayerAction::REMOVE_PIECE:
        out.write(action_info, 2 * sizeof(action_info[0]));
        break;
    }
  }
}

}  // anonymous namespace

// static
void GameSerializer::SerializeTo(const Game& game,
                                 std::ostream& out,
                                 bool use_binary) {
  // TODO(serialization): Add version number
  std::vector<PlayerAction> actions;
  game.DumpActionList(&actions);
  if (use_binary) {
    DCHECK_EQ(sizeof(char), 1);  // NOLINT(runtime/sizeof)
    DCHECK_LT(game.board().size(), std::numeric_limits<char>().max());
    SerializeActionsToBinaryStream(actions, out);
  }
}

}  // namespace game
