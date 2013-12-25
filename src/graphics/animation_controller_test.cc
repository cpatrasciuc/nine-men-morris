// Copyright (c) 2013 Cristian Patrasciuc. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "graphics/animation_controller.h"
#include "graphics/in_game_test.h"
#include "gtest/gtest.h"

#include "OGRE/OgreSceneManager.h"
#include "OGRE/OgreVector3.h"

namespace graphics {
namespace {

class AnimationControllerTest: public InGameTestBase {
 public:
  virtual void TestMethod() {
    Ogre::SceneManager* const scene_manager = app()->scene_manager();
    node_ = scene_manager->getRootSceneNode()->createChildSceneNode("TestNode");
    EXPECT_EQ(Ogre::Vector3::ZERO, node_->getPosition());

    const int iter1 = RunTest(false, 0);
    const int iter2 = RunTest(true, 20);
    const int iter3 = RunTest(true, 10);

    EXPECT_LT(iter1, iter2);
    EXPECT_LT(iter2, iter3);

    SUCCEED();
  }

  int RunTest(bool animations_enabled, double speed) {
    node_->setPosition(Ogre::Vector3::ZERO);
    node_->setVisible(true, true);

    AnimationController animation_controller;
    animation_controller.set_animations_enabled(animations_enabled);
    animation_controller.set_animation_speed(speed);

    const Ogre::Vector3 destination(-50, -50, -50);
    animation_controller.AddMoveAnimation(node_, Ogre::Vector3(100, 100, 100));
    animation_controller.AddRemoveAnimation(node_);
    animation_controller.AddMoveAnimation(node_, destination);

    int iterations = 0;
    while (animation_controller.HasPendingAnimations()) {
      animation_controller.Update(1);
      ++iterations;
    }

    EXPECT_TRUE(node_->getPosition().positionEquals(destination, 0.001));
    return iterations;
  }

 private:
  Ogre::SceneNode* node_;
};

IN_GAME_TEST(AnimationControllerTest, AnimationControllerTest);

}  // anonymous namespace
}  // namespace graphics
