// Copyright (c) 2013 Cristian Patrasciuc. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include <queue>
#include <utility>

#include "graphics/animation_controller.h"

#include "OGRE/OgreSceneNode.h"
#include "OGRE/OgreVector3.h"

namespace graphics {
namespace {

const Ogre::Vector3 kRemovePieceLocation(Ogre::Vector3::NEGATIVE_UNIT_Y);

}  // anonymous namespace

AnimationController::AnimationController()
    : animations_enabled_(false), animation_speed_(25.0) {}

void AnimationController::AddMoveAnimation(Ogre::SceneNode* node,
                                           const Ogre::Vector3& destination) {
  if (animations_enabled_) {
    animated_pieces_.push(std::make_pair(node, destination));
  } else {
    node->setPosition(destination);
  }
}

void AnimationController::AddRemoveAnimation(Ogre::SceneNode* node) {
  // TODO(animation_controller): Replace current hack with different animations
  // types.
  if (animations_enabled_) {
    animated_pieces_.push(std::make_pair(node, kRemovePieceLocation));
  } else {
    node->setVisible(false, true);
  }
}

void AnimationController::Update(double time_delta) {
  if (animated_pieces_.empty()) {
    return;
  }
  bool is_animation_over = false;
  Ogre::SceneNode* const piece_node = animated_pieces_.front().first;
  const Ogre::Vector3 destination = animated_pieces_.front().second;
  if (destination == kRemovePieceLocation) {
    piece_node->setVisible(false, true);
    is_animation_over = true;
  } else {
    Ogre::Vector3 old_position = piece_node->getPosition();
    Ogre::Vector3 distance = destination - old_position;
    Ogre::Vector3 direction = distance.normalisedCopy();
    double distance_to_move = animation_speed_ * time_delta;
    Ogre::Vector3 new_position = old_position + (distance_to_move * direction);
    Ogre::Vector3 new_distance = new_position - old_position;
    if (new_distance.squaredLength() > distance.squaredLength()) {
      new_position = destination;
    }
    if (new_position.squaredDistance(destination) < 0.001) {
      new_position = destination;
      is_animation_over = true;
    }
    piece_node->setPosition(new_position);
  }
  if (is_animation_over) {
    animated_pieces_.pop();
  }
}

}  // namespace graphics
