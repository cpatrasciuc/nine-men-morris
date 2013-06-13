// Copyright (c) 2013 Cristian Patrasciuc. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef GRAPHICS_PLAYER_DELEGATE_H_
#define GRAPHICS_PLAYER_DELEGATE_H_

#include "base/basic_macros.h"
#include "base/callable.h"
#include "game/player_action.h"
#include "graphics/graphics_export.h"

namespace game {
class Game;
}

namespace graphics {

typedef base::Callable<void(const game::PlayerAction&)> PlayerActionCallback;

class GRAPHICS_EXPORT PlayerDelegate {
 public:
  virtual ~PlayerDelegate();

  virtual void RequestAction(const game::Game& game_model,
                             PlayerActionCallback* callback) = 0;

 protected:
  explicit PlayerDelegate(game::PieceColor color);

  game::PieceColor color() const { return color_; }

 private:
  game::PieceColor color_;

  DISALLOW_COPY_AND_ASSIGN(PlayerDelegate);
};

}  // namespace graphics

#endif  // GRAPHICS_PLAYER_DELEGATE_H_

