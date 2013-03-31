// Copyright (c) 2013 Cristian Patrasciuc. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "graphics/game_state.h"

#include "graphics/ogre_app.h"

namespace graphics {

GameState::GameState(OgreApp* app) : app_(app) {}

GameState::~GameState() {}

bool GameState::Initialize() { return true; }

void GameState::Exit() {}

void GameState::Pause() {}

void GameState::Resume() {}

void GameState::Render() {}

}  // namespace graphics
