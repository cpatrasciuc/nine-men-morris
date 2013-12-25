// Copyright (c) 2013 Cristian Patrasciuc. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef GRAPHICS_ANIMATION_CONTROLLER_H_
#define GRAPHICS_ANIMATION_CONTROLLER_H_

#include <queue>
#include <utility>

#include "OGRE/OgreSceneNode.h"
#include "OGRE/OgreVector3.h"

#include "graphics/graphics_export.h"

namespace graphics {

// Class used to animate the pieces on the board based on the player moves. The
// animations are played in the order the moves are performed.
class GRAPHICS_EXPORT AnimationController {
 public:
  AnimationController();

  // If this is |false|, pieces are moved directly on the board instead of
  // using a transition animation towards their destination.
  bool animations_enabled() const { return animations_enabled_; }
  void set_animations_enabled(bool enabled) { animations_enabled_ = enabled; }

  // The speed at which pieces are moved in units/second.
  double animation_speed() const { return animation_speed_; }
  void set_animation_speed(double speed) { animation_speed_ = speed; }

  bool HasPendingAnimations() const { return !animated_pieces_.empty(); }

  void AddMoveAnimation(Ogre::SceneNode* node,
                        const Ogre::Vector3& destination);
  void AddRemoveAnimation(Ogre::SceneNode* node);

  // Updates the current animation, if any. If the current animation is over, it
  // pops it out from the animation queue.
  void Update(double time_delta);

 private:
  // Queue that holds the scene nodes that are currently animated and their
  // location which they are moving towards.
  std::queue<std::pair<Ogre::SceneNode*, Ogre::Vector3> > animated_pieces_;
  bool animations_enabled_;

  double animation_speed_;
};

}  // namespace graphics

#endif  // GRAPHICS_ANIMATION_CONTROLLER_H_

