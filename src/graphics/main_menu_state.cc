// Copyright (c) 2013 Cristian Patrasciuc. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "graphics/main_menu_state.h"

#include <memory>
#include <string>

#include "base/ptr/scoped_ptr.h"
#include "game/game.h"
#include "game/game_options.h"
#include "game/game_type.h"
#include "game/piece_color.h"
#include "graphics/ai_player.h"
#include "graphics/confirmation_menu_state.h"
#include "graphics/game_over_state.h"
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
    Reset(quit_confirmation_state_,
          new ConfirmationMenuState("Are you sure you want to quit?"));
    app()->PushState(Get(quit_confirmation_state_));
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

void MainMenuState::Resume() {
  MenuState::Resume();
  if (quit_confirmation_state_) {
    const bool quit_was_confirmed = quit_confirmation_state_->is_confirmed();
    Reset(quit_confirmation_state_);
    if (quit_was_confirmed) {
      app()->PopState();
    }
  } else if (playing_state_) {
    const bool is_game_over = playing_state_->game_model().is_game_over();
    if (is_game_over) {
      const game::PieceColor winner = playing_state_->game_model().winner();
      Reset(game_over_state_, new GameOverState(winner));
    }
    Reset(playing_state_);
    if (is_game_over) {
      app()->PushState(Get(game_over_state_));
    }
  } else if (game_over_state_) {
    Reset(game_over_state_);
  }
}

}  // namespace graphics
