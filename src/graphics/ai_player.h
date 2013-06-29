// Copyright (c) 2013 Cristian Patrasciuc. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef GRAPHICS_AI_PLAYER_H_
#define GRAPHICS_AI_PLAYER_H_

#include <memory>

#include "ai/ai_algorithm.h"
#include "base/basic_macros.h"
#include "base/ptr/scoped_ptr.h"
#include "graphics/graphics_export.h"
#include "graphics/player_delegate.h"

#include "OGRE/OgreWorkQueue.h"

namespace game {
class Game;
}

namespace graphics {

class GRAPHICS_EXPORT AIPlayer
    : public PlayerDelegate,
      public Ogre::WorkQueue::RequestHandler,
      public Ogre::WorkQueue::ResponseHandler {
 public:
  AIPlayer();
  virtual ~AIPlayer();

 private:
  // PlayerDelegate interface
  virtual void RequestAction(const game::Game& game_model,
                             std::auto_ptr<PlayerActionCallback> callback);

  // Ogre::RequestHandler interface
  virtual Ogre::WorkQueue::Response* handleRequest(
      const Ogre::WorkQueue::Request* request,
      const Ogre::WorkQueue* source_queue);

  // Ogre::ResponseHandler interface
  virtual void handleResponse(const Ogre::WorkQueue::Response* response,
                              const Ogre::WorkQueue* source_queue);

  std::auto_ptr<PlayerActionCallback> callback_;
  base::ptr::scoped_ptr<ai::AIAlgorithm> algorithm_;
  const Ogre::uint16 channel_;
  bool waiting_for_response_;

  DISALLOW_COPY_AND_ASSIGN(AIPlayer);
};

}  // namespace graphics

#endif  // GRAPHICS_AI_PLAYER_H_

