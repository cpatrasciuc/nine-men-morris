// Copyright (c) 2013 Cristian Patrasciuc. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef GRAPHICS_HUMAN_PLAYER_H_
#define GRAPHICS_HUMAN_PLAYER_H_

#include "base/basic_macros.h"
#include "base/ptr/scoped_ptr.h"
#include "game/piece_color.h"
#include "game/player_action.h"
#include "graphics/graphics_export.h"
#include "graphics/player_delegate.h"
#include "graphics/board_renderer.h"

namespace graphics {

class GRAPHICS_EXPORT HumanPlayer
    : public PlayerDelegate, public SelectionListener {
 public:
  explicit HumanPlayer(game::PieceColor color);

  void set_board_view(BoardRenderer* view) { view_ = view; }

 private:
  // PlayerDelegate interface
  virtual void RequestAction(const game::Game& game_model,
                             PlayerActionCallback* callback);

  // BoardRenderer::SelectionListener interface
  virtual void OnLocationSelected(const game::BoardLocation& location);
  virtual void OnSelectionCleared();

  BoardRenderer* view_;

  base::ptr::scoped_ptr<game::PlayerAction> action_;

  DISALLOW_COPY_AND_ASSIGN(HumanPlayer);
};

}  // namespace graphics

#endif  // GRAPHICS_HUMAN_PLAYER_H_

