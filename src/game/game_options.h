// Copyright (c) 2012 Cristian Patrasciuc. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef GAME_GAME_OPTIONS_H_
#define GAME_GAME_OPTIONS_H_

#include "game/game_export.h"
#include "game/game_type.h"

namespace game {

// Class used to group together all the configurable parameters of a game.
class GAME_EXPORT GameOptions {
 public:
  // Initializes a new object with the default game options.
  GameOptions();

  // Specifies one of the allowed board setups. Default: NINE_MEN_MORRIS.
  GameType game_type() const { return game_type_; }
  void set_game_type(GameType type) { game_type_ = type; }

  // Specifies if jumps are allowed when a player remains with less then three
  // pieces on the board. Default: true.
  bool jumps_allowed() const { return jumps_allowed_; }
  void set_jumps_allowed(bool allow) { jumps_allowed_ = allow; }

  // Specifies which player is the first to move. If |white_starts_| is true,
  // the white player will move first, otherwise the black player will move
  // first. Default: true.
  bool white_starts() const { return white_starts_; }
  void set_white_starts(bool white_starts) { white_starts_ = white_starts; }

  bool operator==(const GameOptions& other) const;
  bool operator!=(const GameOptions& other) const;

 private:
  GameType game_type_;
  bool jumps_allowed_;
  bool white_starts_;
};

}  // namespace game

#endif  // GAME_GAME_OPTIONS_H_

