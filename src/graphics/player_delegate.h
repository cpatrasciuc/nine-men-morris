// Copyright (c) 2013 Cristian Patrasciuc. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef GRAPHICS_PLAYER_DELEGATE_H_
#define GRAPHICS_PLAYER_DELEGATE_H_

#include <memory>

#include "base/basic_macros.h"
#include "base/callable.h"
#include "game/player_action.h"
#include "graphics/graphics_export.h"

namespace game {
class Game;
}

namespace graphics {

class PlayingState;

typedef base::Callable<void(const game::PlayerAction&)> PlayerActionCallback;

class GRAPHICS_EXPORT PlayerDelegate {
 public:
  virtual ~PlayerDelegate();

  virtual void Initialize(PlayingState* state);

  virtual void RequestAction(const game::Game& game_model,
                             std::auto_ptr<PlayerActionCallback> callback) = 0;

  game::PieceColor color() const { return color_; }
  void set_color(game::PieceColor color) { color_ = color; }

 protected:
  PlayerDelegate();

 private:
  game::PieceColor color_;

  DISALLOW_COPY_AND_ASSIGN(PlayerDelegate);
};

}  // namespace graphics

#endif  // GRAPHICS_PLAYER_DELEGATE_H_

