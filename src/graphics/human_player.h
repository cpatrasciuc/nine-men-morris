// Copyright (c) 2013 Cristian Patrasciuc. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef GRAPHICS_HUMAN_PLAYER_H_
#define GRAPHICS_HUMAN_PLAYER_H_

#include <memory>

#include "base/basic_macros.h"
#include "base/ptr/scoped_ptr.h"
#include "game/player_action.h"
#include "graphics/graphics_export.h"
#include "graphics/player_delegate.h"
#include "graphics/selection_listener.h"

namespace game {
class Game;
}

namespace graphics {

class BoardView;
class PlayingState;

class GRAPHICS_EXPORT HumanPlayer
    : public PlayerDelegate, public SelectionListener {
 public:
  HumanPlayer();

  virtual void Initialize(PlayingState* state);

 private:
  // PlayerDelegate interface
  virtual void RequestAction(const game::Game& game_model,
                             std::auto_ptr<PlayerActionCallback> callback);

  // BoardRenderer::SelectionListener interface
  virtual void OnLocationSelected(const game::BoardLocation& location);

  void ExecuteAction();

  BoardView* view_;

  base::ptr::scoped_ptr<game::PlayerAction> action_;
  std::auto_ptr<PlayerActionCallback> callback_;

  DISALLOW_COPY_AND_ASSIGN(HumanPlayer);
};

}  // namespace graphics

#endif  // GRAPHICS_HUMAN_PLAYER_H_

