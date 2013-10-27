// Copyright (c) 2013 Cristian Patrasciuc. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "game/game_listener.h"

#include "game/piece_color.h"
#include "game/player_action.h"

namespace game {

GameListener::GameListener() {}

GameListener::~GameListener() {}

void GameListener::OnGameInitialized() {}

void GameListener::OnPlayerAction(const PlayerAction& action) {}

void GameListener::OnUndoPlayerAction(const PlayerAction& action) {}

void GameListener::OnGameOver(PieceColor winner) {}

}  // namespace game
