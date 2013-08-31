// Copyright (c) 2013 Cristian Patrasciuc. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "graphics/main_menu_state.h"

#include <memory>
#include <string>

#include "game/game.h"
#include "game/game_options.h"
#include "game/game_type.h"
#include "graphics/ai_player.h"
#include "graphics/human_player.h"
#include "graphics/menu_state.h"
#include "graphics/ogre_app.h"
#include "graphics/player_delegate.h"

namespace graphics {

const char kNewGameOption[] = "New Game";
const char kQuitOption[] = "Quit";

MainMenuState::MainMenuState() : MenuState("MainMenu") {
  set_escape_option(kQuitOption);
}

void MainMenuState::OnMenuOptionSelected(const std::string& option) {
  if (option == kQuitOption) {
    // TODO(main_menu): Add user confirmation.
    app()->PopState();
    return;
  }
  if (option == kNewGameOption) {
    game::GameOptions options;
    options.set_jumps_allowed(false);
    options.set_game_type(game::THREE_MEN_MORRIS);
    std::auto_ptr<game::Game> game_model(new game::Game(options));
    game_model->Initialize();
    PlayingState* const state = new PlayingState(game_model,
      std::auto_ptr<graphics::PlayerDelegate>(new graphics::HumanPlayer()),
      std::auto_ptr<graphics::PlayerDelegate>(new graphics::AIPlayer()));
    Reset(playing_state_, state);
    app()->PushState(state);
  }
}

}  // namespace graphics
