// Copyright (c) 2012 Cristian Patrasciuc. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "game/player.h"

#include "game/board.h"
#include "game/game.h"

namespace game {

Player::Player(const std::string& name)
    : name_(name), color_(Board::NO_COLOR), game_(NULL) {}

Player::~Player() {}

}  // namespace game
