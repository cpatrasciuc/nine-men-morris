// Copyright (c) 2013 Cristian Patrasciuc. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "game/piece_color.h"
#include "graphics/player_delegate.h"
#include "graphics/playing_state.h"

namespace graphics {

PlayerDelegate::PlayerDelegate() : color_(game::NO_COLOR) {}

PlayerDelegate::~PlayerDelegate() {}

void PlayerDelegate::Initialize(PlayingState* state) {}

}  // namespace graphics
