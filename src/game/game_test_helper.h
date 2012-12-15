// Copyright (c) 2012 Cristian Patrasciuc. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef GAME_GAME_TEST_HELPER_H_
#define GAME_GAME_TEST_HELPER_H_

#include <memory>
#include <string>

#include "game/game_export.h"

namespace game {

class Game;

// Deserializes a saved game that is supposed to be used in unit tests.
// In addition to a normal saved game, this file may contain developer comments,
// represented by lines starting with #. The location where these saved games
// are stored is kept private. The client should not need it. It is computed
// internally based on the provided |game_name|.
GAME_EXPORT
std::auto_ptr<Game> LoadSavedGameForTests(const std::string& game_name);

}  // namespace game

#endif  // GAME_GAME_TEST_HELPER_H_

