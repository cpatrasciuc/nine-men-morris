// Copyright (c) 2013 Cristian Patrasciuc. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "game/game_options.h"
#include "game/game_type.h"

#include "graphics/ogre_app.h"
#include "graphics/game_state.h"
#include "graphics/playing_state.h"

#include "OIS/OISKeyboard.h"

namespace {

class EmptyGameState : public graphics::GameState {
 public:
  explicit EmptyGameState(graphics::OgreApp* app) : GameState(app) {}

  bool keyPressed(const OIS::KeyEvent& event) {
    if (event.key == OIS::KC_ESCAPE) {
      app()->PopState();
    }
    return true;
  }
};

}  // anonymous namespace

int main(int argc, char** argv) {
  graphics::OgreApp app("Temp test for OgreApp");
  app.Init();
  game::GameOptions options;
  options.set_game_type(game::SIX_MEN_MORRIS);
  // EmptyGameState game_state(&app);
  graphics::PlayingState game_state(&app, options);
  app.PushState(&game_state);
  app.RunMainLoop();
  return 0;
}
