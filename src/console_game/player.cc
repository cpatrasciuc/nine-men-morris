// Copyright (c) 2013 Cristian Patrasciuc. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include <string>

#include "console_game/player.h"

namespace console_game {

Player::Player(const std::string& name, const game::PieceColor& color)
    : name_(name), color_(color) {}

Player::~Player() {}

}  // namespace console_game
