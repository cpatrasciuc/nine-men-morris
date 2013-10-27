// Copyright (c) 2013 Cristian Patrasciuc. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef GAME_GAME_LISTENER_H_
#define GAME_GAME_LISTENER_H_

#include "base/basic_macros.h"

#include "game/game_export.h"
#include "game/piece_color.h"

namespace game {

class PlayerAction;

// The class can be used to monitor various events during the course of a game.
// The default implementation is an empty one. The clients can extend this class
// and override only specific methods.
// See Game::AddListener() or Game::RemoveListener().
class GAME_EXPORT GameListener {
 public:
  GameListener();
  virtual ~GameListener();

  // This method is called after the game instance is initialized.
  virtual void OnGameInitialized();

  // This method is called after a new player action is executed. The caller
  // should not store a reference to |action|.
  virtual void OnPlayerAction(const PlayerAction& action);

  // This method is called after a player action is undone. The caller should
  // not store a reference to |action|.
  virtual void OnUndoPlayerAction(const PlayerAction& action);

  // This method is called when the game is over.
  virtual void OnGameOver(PieceColor winner);

 private:
  DISALLOW_COPY_AND_ASSIGN(GameListener);
};

}  // namespace game

#endif  // GAME_GAME_LISTENER_H_

