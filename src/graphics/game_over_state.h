// Copyright (c) 2013 Cristian Patrasciuc. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef GRAPHICS_GAME_OVER_STATE_H_
#define GRAPHICS_GAME_OVER_STATE_H_

#include <string>

#include "game/piece_color.h"
#include "graphics/graphics_export.h"
#include "graphics/menu_state.h"

namespace graphics {

// The game state used when a game is over. It specifies the winner and has a
// single OK button to exit the state. It uses the game_over.overlay script.
class GRAPHICS_EXPORT GameOverState : public MenuState {
 public:
  explicit GameOverState(game::PieceColor winner);

  // MenuState overrides
  virtual bool Initialize();
  virtual void OnMenuOptionSelected(const std::string& option);

 private:
  game::PieceColor winner_;
};

}  // namespace graphics

#endif  // GRAPHICS_GAME_OVER_STATE_H_

