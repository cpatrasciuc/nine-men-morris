// Copyright (c) 2013 Cristian Patrasciuc. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "graphics/camera_controller.h"

#include "OGRE/OgreCamera.h"
#include "OGRE/OgreMath.h"
#include "OGRE/OgreVector3.h"

#include "OIS/OISMouse.h"

namespace graphics {

CameraController::CameraController()
    : camera_(NULL),
      min_distance_(0),
      max_distance_(1e9),
      zoom_speed_(1.0),
      orbit_speed_(1.0),
      orbiting_(false) {}

void CameraController::mouseMoved(const OIS::MouseEvent& event) {
  if (!camera_) {
    return;
  }
  if (event.state.Z.rel != 0) {
    const Ogre::Vector3 camera_position = camera_->getPosition();
    Ogre::Vector3 delta(0, 0, event.state.Z.rel * zoom_speed_);
    const Ogre::Vector3 new_position = camera_position + delta;
    if (event.state.Z.rel > 0 && new_position.length() > max_distance_) {
      camera_->setPosition(Ogre::Vector3::ZERO);
      delta = Ogre::Vector3(0, 0, max_distance_);
    }
    if (event.state.Z.rel < 0 && new_position.length() < min_distance_) {
      camera_->setPosition(Ogre::Vector3::ZERO);
      delta = Ogre::Vector3(0, 0, min_distance_);
    }
    camera_->moveRelative(delta);
  }
  if (orbiting_) {
    const double target_distance = camera_->getPosition().length();
    Ogre::Degree pitch_delta(-event.state.Y.rel * orbit_speed_);
    if (pitch_delta.valueDegrees() > 5) {
      pitch_delta = 5;
    }
    if (pitch_delta.valueDegrees() < -5) {
      pitch_delta = -5;
    }
    camera_->setPosition(Ogre::Vector3::ZERO);
    camera_->pitch(pitch_delta);
    camera_->yaw(Ogre::Degree(-event.state.X.rel * orbit_speed_));
    camera_->moveRelative(Ogre::Vector3(0, 0, target_distance));
    const double pitch = camera_->getOrientation().getPitch().valueDegrees();
    if ((pitch > -10 && pitch < 10) || (pitch > 80) || (pitch < -80)) {
      camera_->setPosition(Ogre::Vector3::ZERO);
      camera_->pitch(-pitch_delta);
      camera_->moveRelative(Ogre::Vector3(0, 0, target_distance));
    }
  }
}

void CameraController::mousePressed(const OIS::MouseEvent& event,
                                    OIS::MouseButtonID id) {
  if (id == OIS::MB_Right) {
    orbiting_ = true;
  }
}

void CameraController::mouseReleased(const OIS::MouseEvent& event,
                                     OIS::MouseButtonID id) {
  if (id == OIS::MB_Right) {
    orbiting_ = false;
  }
}

}  // namespace graphics
