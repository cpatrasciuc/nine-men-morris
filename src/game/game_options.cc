// Copyright (c) 2012 Cristian Patrasciuc. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "game/game_options.h"

#include "game/game_type.h"

namespace game {

GameOptions::GameOptions()
    : game_type_(NINE_MEN_MORRIS),
      jumps_allowed_(true),
      white_starts_(true) {
}

bool GameOptions::operator==(const GameOptions& other) const {
  return (game_type_ == other.game_type_) &&
         (jumps_allowed_ == other.jumps_allowed_) &&
         (white_starts_ == other.white_starts_);
}

bool GameOptions::operator !=(const GameOptions& other) const {
  return !((*this) == other);
}

}  // namespace game
