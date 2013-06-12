// Copyright (c) 2013 Cristian Patrasciuc. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "game/game.h"
#include "game/player_action.h"
#include "graphics/player_delegate.h"

#include "OGRE/OgreAny.h"
#include "OGRE/OgreWorkQueue.h"

namespace graphics {

PlayerDelegate::PlayerDelegate(game::PieceColor color) : color_(color) {}

PlayerDelegate::~PlayerDelegate() {}

/*Ogre::WorkQueue::Response* PlayerDelegate::handleRequest(
    const Ogre::WorkQueue::Request* request, const Ogre::WorkQueue* queue) {
  const game::Game& game_model = request->getData().get<const game::Game&>();
  game::PlayerAction action = PlayerActionRequested(game_model);
  return OGRE_NEW Ogre::WorkQueue::Response(request, true, Ogre::Any(action));
}*/

}  // namespace graphics
