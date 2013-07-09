// Copyright (c) 2013 Cristian Patrasciuc. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "base/log.h"
#include "graphics/camera_controller.h"
#include "graphics/in_game_test.h"
#include "gtest/gtest.h"

#include "OGRE/OgreCamera.h"
#include "OGRE/OgreColourValue.h"
#include "OGRE/OgreCommon.h"
#include "OGRE/OgreEntity.h"
#include "OGRE/OgreSceneManager.h"
#include "OGRE/OgreVector3.h"

#include "OIS/OISMouse.h"

namespace graphics {
namespace {

class CameraControllerTest : public InGameTestBase {
 public:
  void SetupScene() {
    Ogre::SceneManager* const scene_manager = app()->scene_manager();
    Ogre::Entity* const cube_entity =
        scene_manager->createEntity("Cube", Ogre::SceneManager::PT_CUBE);
    scene_manager->getRootSceneNode()->createChildSceneNode()
        ->attachObject(cube_entity);
    scene_manager->setAmbientLight(Ogre::ColourValue::Red);
  }

  void ResetCamera() {
    Ogre::Camera* const camera = app()->camera();
    camera->setPosition(default_camera_position_);
    camera->lookAt(0, 0, 0);
    camera->setPolygonMode(Ogre::PM_WIREFRAME);
  }

  void TestMethod() {
    SetupScene();
    camera_controller_.set_camera(app()->camera());
    RunZoomTests();
    RunOrbitTests();
    SUCCEED();
  }

 private:
  void RunZoomTests() {
    Ogre::Camera* const camera = camera_controller_.camera();
    default_camera_position_ = Ogre::Vector3(500, 500, 500);

    ResetCamera();
    Ogre::Vector3 initial = camera->getPosition();
    SimulateMouseScroll(-10);
    EXPECT_GT(initial.length(), camera->getPosition().length());

    ResetCamera();
    initial = camera->getPosition();
    SimulateMouseScroll(10);
    EXPECT_LT(initial.length(), camera->getPosition().length());

    const double low_speed_length = camera->getPosition().length();
    const double old_zoom_speed = camera_controller_.zoom_speed();
    ResetCamera();
    camera_controller_.set_zoom_speed(2.0);
    SimulateMouseScroll(10);
    const double high_speed_length = camera->getPosition().length();
    EXPECT_LT(low_speed_length, high_speed_length);
    camera_controller_.set_zoom_speed(old_zoom_speed);

    ResetCamera();
    initial = camera->getPosition();
    const double old_min = camera_controller_.min_distance();
    const double old_max = camera_controller_.max_distance();
    camera_controller_.set_min_distance(initial.length());
    camera_controller_.set_max_distance(initial.length());
    SimulateMouseScroll(10);
    EXPECT_FLOAT_EQ(initial.length(), camera->getPosition().length());
    SimulateMouseScroll(-10);
    EXPECT_FLOAT_EQ(initial.length(), camera->getPosition().length());
    camera_controller_.set_min_distance(old_min);
    camera_controller_.set_max_distance(old_max);

    camera_controller_.set_camera(NULL);
    SimulateMouseScroll(10);
    camera_controller_.set_camera(camera);
  }

  void RunOrbitTests() {
    Ogre::Camera* const camera = camera_controller_.camera();
    default_camera_position_ = Ogre::Vector3(0, 500, 500);

    ResetCamera();
    Ogre::Vector3 initial = camera->getPosition();
    SimulateMouseDrag(10, 0);
    EXPECT_GT(initial.x, camera->getPosition().x);
    EXPECT_NEAR(initial.y, camera->getPosition().y, 0.001);
    EXPECT_GT(initial.z, camera->getPosition().z);

    ResetCamera();
    initial = camera->getPosition();
    SimulateMouseDrag(-10, 0);
    EXPECT_LT(initial.x, camera->getPosition().x);
    EXPECT_NEAR(initial.y, camera->getPosition().y, 0.001);
    EXPECT_GT(initial.z, camera->getPosition().z);

    ResetCamera();
    initial = camera->getPosition();
    SimulateMouseDrag(0, 10);
    EXPECT_NEAR(initial.x, camera->getPosition().x, 0.001);
    EXPECT_LT(initial.y, camera->getPosition().y);
    EXPECT_GT(initial.z, camera->getPosition().z);

    ResetCamera();
    initial = camera->getPosition();
    SimulateMouseDrag(0, -10);
    EXPECT_NEAR(initial.x, camera->getPosition().x, 0.001);
    EXPECT_GT(initial.y, camera->getPosition().y);
    EXPECT_LT(initial.z, camera->getPosition().z);

    ResetCamera();
    initial = camera->getPosition();
    const double old_speed = camera_controller_.orbit_speed();
    SimulateMouseDrag(5, 5);
    const Ogre::Vector3 low_speed_position = camera->getPosition();
    ResetCamera();
    camera_controller_.set_orbit_speed(2 * old_speed);
    SimulateMouseDrag(5, 5);
    const Ogre::Vector3 high_speed_position = camera->getPosition();
    EXPECT_LT((low_speed_position - initial).length(),
              (high_speed_position - initial).length());

    ResetCamera();
    initial = camera->getPosition();
    SimulateMouseDrag(10, -10, OIS::MB_Left);
    EXPECT_TRUE(camera->getPosition().positionEquals(initial, 0.001));

    camera_controller_.set_camera(NULL);
    SimulateMouseDrag(10, -10);
    camera_controller_.set_camera(camera);
  }

  void SimulateMouseScroll(int scroll_amount) {
    OIS::MouseState state;
    state.Z.rel = scroll_amount;
    const OIS::MouseEvent event(NULL, state);
    camera_controller_.mouseMoved(event);
  }

  void SimulateMouseDrag(int dx, int dy,
                         OIS::MouseButtonID button_id = OIS::MB_Right) {
    OIS::MouseState state;
    const OIS::MouseEvent button_pressed_event(NULL, state);
    camera_controller_.mousePressed(button_pressed_event, button_id);
    state.X.rel = dx;
    state.Y.rel = dy;
    const OIS::MouseEvent moused_moved_event(NULL, state);
    camera_controller_.mouseMoved(moused_moved_event);
    state.clear();
    const OIS::MouseEvent button_released_event(NULL, state);
    camera_controller_.mouseReleased(button_released_event, button_id);
  }

  CameraController camera_controller_;
  Ogre::Vector3 default_camera_position_;
};

IN_GAME_TEST(CameraControllerTest, Test);

}  // anonymous namespace
}  // namespace graphics
