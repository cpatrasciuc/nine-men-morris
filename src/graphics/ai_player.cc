// Copyright (c) 2013 Cristian Patrasciuc. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "graphics/ai_player.h"

#include <memory>

#include "ai/alphabeta/morris_alphabeta.h"
#include "base/ptr/scoped_ptr.h"
#include "game/game.h"
#include "game/piece_color.h"
#include "game/player_action.h"

#include "OGRE/OgreAny.h"
#include "OGRE/OgreRoot.h"
#include "OGRE/OgreWorkQueue.h"

namespace graphics {

AIPlayer::AIPlayer()
    : callback_(NULL),
      algorithm_(NULL),
      channel_(Ogre::Root::getSingleton().getWorkQueue()->getChannel("AI")) {}

void AIPlayer::RequestAction(const game::Game& game_model,
                             std::auto_ptr<PlayerActionCallback> callback) {
  DCHECK(color() != game::NO_COLOR);
  callback_ = callback;
  Ogre::WorkQueue* const work_queue = Ogre::Root::getSingleton().getWorkQueue();
  work_queue->addRequestHandler(channel_, this);
  work_queue->addResponseHandler(channel_, this);
  work_queue->addRequest(channel_, 0, Ogre::Any(&game_model));
}

Ogre::WorkQueue::Response* AIPlayer::handleRequest(
    const Ogre::WorkQueue::Request* request,
    const Ogre::WorkQueue* source_queue) {
  const game::Game* game_model = request->getData().get<const game::Game*>();
  if (!Get(algorithm_)) {
    Reset(algorithm_,
          new ai::alphabeta::MorrisAlphaBeta(game_model->options()));
  }
  const game::PlayerAction* action = new game::PlayerAction(
      algorithm_->GetNextAction(*game_model));
  return OGRE_NEW Ogre::WorkQueue::Response(request, true, Ogre::Any(action));
}

void AIPlayer::handleResponse(const Ogre::WorkQueue::Response* response,
                              const Ogre::WorkQueue* source_queue) {
  DCHECK(response->succeeded());
  DCHECK(callback_.get());
  base::ptr::scoped_ptr<const game::PlayerAction> action(
      response->getData().get<const game::PlayerAction*>());
  Ogre::WorkQueue* const work_queue = Ogre::Root::getSingleton().getWorkQueue();
  work_queue->removeRequestHandler(channel_, this);
  work_queue->removeResponseHandler(channel_, this);
  (*callback_)(*action);
}

}  // namespace graphics
