// Copyright (c) 2013 Cristian Patrasciuc. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "graphics/playing_state.h"

#include <memory>

#include "base/bind.h"
#include "base/log.h"
#include "base/method.h"
#include "base/ptr/scoped_ptr.h"
#include "game/game.h"
#include "game/player_action.h"
#include "graphics/ai_player.h"
#include "graphics/board_view.h"
#include "graphics/human_player.h"
#include "graphics/ogre_app.h"
#include "graphics/player_delegate.h"

#include "OGRE/OgreSceneManager.h"
#include "OGRE/OgreSceneNode.h"

namespace graphics {

PlayingState::PlayingState(OgreApp* app, std::auto_ptr<game::Game> game_model)
    : GameState(app),
      game_(game_model.release()),
      board_view_(new BoardView(app, *game_)),
      camera_controller_(),
      white_player_(NULL),
      black_player_(NULL) {}

bool PlayingState::Initialize() {
  board_view_->Initialize();
  camera_controller_.set_min_distance(50);
  camera_controller_.set_max_distance(200);
  camera_controller_.set_camera(app()->camera());
  game_->AddListener(Get(board_view_));
  InitializePlayers();
  RequestPlayerAction();
  return true;
}

void PlayingState::Exit() {
  camera_controller_.set_camera(NULL);
  Reset(board_view_);
  Reset(white_player_);
  Reset(black_player_);
  Ogre::SceneManager* const scene_manager = app()->scene_manager();
  Ogre::SceneNode* const root = scene_manager->getRootSceneNode();
  root->removeAllChildren();
}

bool PlayingState::keyPressed(const OIS::KeyEvent& event) {
  if (event.key == OIS::KC_ESCAPE) {
    app()->PopState();
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

void PlayingState::InitializePlayers() {
  HumanPlayer* human_player = new HumanPlayer(game::WHITE_COLOR);
  human_player->set_board_view(Get(board_view_));
  Reset(white_player_, human_player);
  Reset(black_player_, new AIPlayer(game::BLACK_COLOR));
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
  if (!game_->is_game_over()) {
    RequestPlayerAction();
  }
}

}  // namespace graphics
