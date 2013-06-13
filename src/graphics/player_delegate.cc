// Copyright (c) 2013 Cristian Patrasciuc. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "game/piece_color.h"
#include "graphics/player_delegate.h"

namespace graphics {

PlayerDelegate::PlayerDelegate(game::PieceColor color) : color_(color) {}

PlayerDelegate::~PlayerDelegate() {}

}  // namespace graphics
