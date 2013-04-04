// Copyright (c) 2013 Cristian Patrasciuc. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef GRAPHICS_PLAYING_STATE_H_
#define GRAPHICS_PLAYING_STATE_H_

#include "game/game.h"
#include "game/game_options.h"
#include "graphics/game_state.h"
#include "graphics/graphics_export.h"

namespace graphics {

class GRAPHICS_EXPORT PlayingState : public GameState {
 public:
  explicit PlayingState(OgreApp* app,
                        game::GameOptions game_options = game::GameOptions());

  virtual bool Initialize();
  virtual void Exit();

 private:
  virtual bool keyPressed(const OIS::KeyEvent& event);

  game::Game game_;
};

}  // namespace graphics

#endif  // GRAPHICS_PLAYING_STATE_H_

