// Copyright (c) 2013 Cristian Patrasciuc. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include <memory>
#include <string>

#include "base/debug/stacktrace.h"
#include "base/log.h"

#include "game/game.h"
#include "game/game_options.h"
#include "game/game_type.h"
#include "game/piece_color.h"

#include "graphics/ai_player.h"
#include "graphics/confirmation_menu_state.h"
#include "graphics/game_over_state.h"
#include "graphics/game_state.h"
#include "graphics/human_player.h"
#include "graphics/main_menu_state.h"
#include "graphics/menu_state.h"
#include "graphics/new_game_menu_state.h"
#include "graphics/ogre_app.h"
#include "graphics/playing_state.h"

#include "OIS/OISKeyboard.h"

namespace {

class EmptyGameState : public graphics::GameState {
 public:
  bool keyPressed(const OIS::KeyEvent& event) {
    if (event.key == OIS::KC_ESCAPE) {
      app()->PopState();
    }
    return true;
  }
};

class SampleMenu : public graphics::MenuState {
 public:
  SampleMenu() : MenuState("TestMenu") {}

  virtual void OnMenuOptionSelected(const std::string& option) {
    LOG(INFO) << option;
    if (option == "Option3") {
      graphics::OgreApp::Instance().PopState();
    }
  }
};

}  // anonymous namespace

int main(int argc, char** argv) {
  base::debug::EnableStackTraceDumpOnCrash();
  graphics::OgreApp& app = graphics::OgreApp::Instance();
  app.Init();
  game::GameOptions options;
  options.set_jumps_allowed(false);
  options.set_game_type(game::THREE_MEN_MORRIS);
  std::auto_ptr<game::Game> game_model(new game::Game(options));
  game_model->Initialize();
  // EmptyGameState game_state(&app);
  /*graphics::PlayingState game_state(
      game_model,
      std::auto_ptr<graphics::PlayerDelegate>(new graphics::HumanPlayer()),
      std::auto_ptr<graphics::PlayerDelegate>(new graphics::AIPlayer()));*/
  graphics::MainMenuState game_state;
  app.PushState(&game_state);
  app.RunMainLoop();
  app.ShutDown();
  return 0;
}
