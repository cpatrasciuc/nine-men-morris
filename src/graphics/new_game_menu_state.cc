// Copyright (c) 2013 Cristian Patrasciuc. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "graphics/new_game_menu_state.h"

#include <string>

#include "base/basic_macros.h"
#include "base/log.h"
#include "game/game_type.h"
#include "graphics/menu_state.h"
#include "graphics/ogre_app.h"
#include "graphics/player_configuration.h"

#include "OGRE/OgreOverlayElement.h"
#include "OGRE/OgreOverlayManager.h"

namespace graphics {
namespace {

// Generic structure used to store info related to each game option.
struct Option {
  // Currently selected value.
  int index;
  // Total number of values.
  const int count;
  // The text messages corresponding to each possible value.
  const char** captions;
  // The integer/enum value corresponding to each value.
  const int* values;
  // The name of the overlay element (menu button) that handles this option.
  const char* overlay_name;
};

// Possible game types.
const char* kGameTypeCaptions[] = { "Three", "Six", "Nine" };
const int kGameTypeValues[] = { game::THREE_MEN_MORRIS,
                                game::SIX_MEN_MORRIS,
                                game::NINE_MEN_MORRIS };
Option g_game_type_option = { 0, arraysize(kGameTypeValues), kGameTypeCaptions,
                              kGameTypeValues, "GameTypeMenuText" };

// Possible player configurations.
const char* kPlayerConfigCaptions[] = { "Human vs. AI",
                                        "AI vs. Human",
                                        "Human vs. Human" };
const int kPlayerTypeValues[] = { HUMAN_VS_AI, AI_VS_HUMAN, HUMAN_VS_HUMAN };
Option g_player_config_option = { 0, arraysize(kPlayerTypeValues),
    kPlayerConfigCaptions, kPlayerTypeValues, "PlayerConfigMenuText" };

// Enable/disable jumps when a player has only three pieces left.
const char* kJumpCaptions[] = { "Jumps allowed", "Jumps not allowed" };
const int kJumpValues[] = { true, false };
Option g_jump_option = { 0, arraysize(kJumpValues), kJumpCaptions, kJumpValues,
                         "JumpsMenuText" };

// The global list of game options.
Option* const g_options[] = { &g_game_type_option,
                              &g_jump_option,
                              &g_player_config_option };

const char kStartGameOption[] = "Start Game!";

}  // anonymous namespace

NewGameMenuState::NewGameMenuState() : MenuState("NewGameMenu") {}

game::GameType NewGameMenuState::game_type() const {
  return static_cast<game::GameType>(
      g_game_type_option.values[g_game_type_option.index]);
}

PlayerConfiguration NewGameMenuState::player_configuration() const {
  return static_cast<PlayerConfiguration>(
      g_player_config_option.values[g_player_config_option.index]);
}

bool NewGameMenuState::jumps_allowed() const {
  return g_jump_option.values[g_jump_option.index];
}

void NewGameMenuState::OnMenuOptionSelected(const std::string& option) {
  if (option == kStartGameOption) {
    app()->PopState();
  }
  for (size_t i = 0; i < arraysize(g_options); ++i) {
    if (option == g_options[i]->captions[g_options[i]->index]) {
      g_options[i]->index = (g_options[i]->index + 1) % g_options[i]->count;
      Ogre::OverlayElement* const overlay_element =
          Ogre::OverlayManager::getSingleton().getOverlayElement(
              g_options[i]->overlay_name);
      DCHECK(overlay_element);
      overlay_element->setCaption(g_options[i]->captions[g_options[i]->index]);
    }
  }
}

// TODO(new_game_menu): Add a Back menu item.

}  // namespace graphics
