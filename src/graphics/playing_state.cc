// Copyright (c) 2013 Cristian Patrasciuc. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "graphics/playing_state.h"

#include <memory>
#include <string>

#include "base/bind.h"
#include "base/log.h"
#include "base/method.h"
#include "base/ptr/scoped_ptr.h"
#include "game/game.h"
#include "game/mill_events_generator.h"
#include "game/player_action.h"
#include "graphics/ai_player.h"
#include "graphics/animation_controller.h"
#include "graphics/board_view.h"
#include "graphics/confirmation_menu_state.h"
#include "graphics/human_player.h"
#include "graphics/ogre_app.h"
#include "graphics/player_delegate.h"

#include "OGRE/OgreFrameListener.h"

namespace graphics {

PlayingState::PlayingState(std::auto_ptr<game::Game> game_model,
                           std::auto_ptr<PlayerDelegate> white_player,
                           std::auto_ptr<PlayerDelegate> black_player)
    : game_(game_model.release()),
      board_view_(new BoardView(*game_)),
      mill_events_generator_(new game::MillEventsGenerator(Get(game_))),
      camera_controller_(),
      white_player_(white_player.release()),
      black_player_(black_player.release()),
      stopped_(false) {
  DCHECK(Get(game_));
  DCHECK(Get(white_player_));
  DCHECK(Get(black_player_));
}

bool PlayingState::Initialize() {
  board_view_->animation_controller()->set_animations_enabled(true);
  board_view_->Initialize();
  mill_events_generator_->AddListener(Get(board_view_));
  camera_controller_.set_min_distance(50);
  camera_controller_.set_max_distance(200);
  camera_controller_.set_zoom_speed(0.05);
  camera_controller_.set_orbit_speed(0.25);
  camera_controller_.set_camera(app()->camera());
  game_->AddListener(Get(board_view_));
  InitializePlayers();
  RequestPlayerAction();
  return true;
}

void PlayingState::Exit() {
  stopped_ = true;
  mill_events_generator_->RemoveListener(Get(board_view_));
  Reset(mill_events_generator_);
  Reset(white_player_);
  Reset(black_player_);
  Reset(board_view_);
  camera_controller_.set_camera(NULL);
}

void PlayingState::Resume() {
  const bool quit = Get(pause_menu_) && pause_menu_->is_confirmed();
  Reset(pause_menu_);
  if (quit) {
    app()->PopState();
  }
}

bool PlayingState::keyReleased(const OIS::KeyEvent& event) {
  if (event.key == OIS::KC_ESCAPE) {
    const std::string msg = "Quit to main menu?\nAll progress will be lost.";
    Reset(pause_menu_, new ConfirmationMenuState(msg));
    app()->PushState(Get(pause_menu_));
  }
  return true;
}

bool PlayingState::mouseMoved(const OIS::MouseEvent& event) {
  camera_controller_.mouseMoved(event);
  board_view_->mouseMoved(event);
  return true;
}

bool PlayingState::mousePressed(const OIS::MouseEvent& event,
                                OIS::MouseButtonID id) {
  camera_controller_.mousePressed(event, id);
  board_view_->mousePressed(event, id);
  return true;
}

bool PlayingState::mouseReleased(const OIS::MouseEvent& event,
                                 OIS::MouseButtonID id) {
  camera_controller_.mouseReleased(event, id);
  board_view_->mouseReleased(event, id);
  return true;
}

bool PlayingState::frameRenderingQueued(const Ogre::FrameEvent& event) {
  AnimationController* controller = board_view_->animation_controller();
  controller->Update(event.timeSinceLastEvent);
  if (game_->is_game_over() && !controller->HasPendingAnimations()) {
    app()->PopState();
  }
  return true;
}

void PlayingState::InitializePlayers() {
  white_player_->set_color(game::WHITE_COLOR);
  white_player_->Initialize(this);
  black_player_->set_color(game::BLACK_COLOR);
  black_player_->Initialize(this);
}

void PlayingState::RequestPlayerAction() {
  typedef void(PlayingState::*ExecuteActionSig)(const game::PlayerAction&);
  PlayerActionCallback* callback = base::Bind(
      new base::Method<ExecuteActionSig>(&PlayingState::ExecuteAction), this);
  PlayerDelegate* player = game_->current_player() == game::WHITE_COLOR ?
      Get(white_player_) : Get(black_player_);
  player->RequestAction(*game_, std::auto_ptr<PlayerActionCallback>(callback));
}

void PlayingState::ExecuteAction(const game::PlayerAction& action) {
  game_->ExecutePlayerAction(action);
  if (!game_->is_game_over() && !stopped_) {
    RequestPlayerAction();
  }
}

}  // namespace graphics
