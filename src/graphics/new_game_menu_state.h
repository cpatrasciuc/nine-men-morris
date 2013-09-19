// Copyright (c) 2013 Cristian Patrasciuc. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef GRAPHICS_NEW_GAME_MENU_STATE_H_
#define GRAPHICS_NEW_GAME_MENU_STATE_H_

#include <string>

#include "game/game_type.h"
#include "graphics/graphics_export.h"
#include "graphics/menu_state.h"
#include "graphics/player_configuration.h"

namespace graphics {

class GRAPHICS_EXPORT NewGameMenuState : public MenuState {
 public:
  NewGameMenuState();

  // Accessors for each game option.
  game::GameType game_type() const;
  PlayerConfiguration player_configuration() const;
  bool jumps_allowed() const;

  // MenuState overrides
  virtual void OnMenuOptionSelected(const std::string& option);
};

}  // namespace graphics

#endif  // GRAPHICS_NEW_GAME_MENU_STATE_H_

