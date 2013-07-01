// Copyright (c) 2013 Cristian Patrasciuc. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef GRAPHICS_CAMERA_CONTROLLER_H_
#define GRAPHICS_CAMERA_CONTROLLER_H_

#include "base/basic_macros.h"
#include "graphics/graphics_export.h"

#include "OIS/OISMouse.h"

namespace Ogre {
class Camera;
}

namespace graphics {

// This class handles mouse events and lets the user navigate around the game
// board. The camera will always look at the origin (0, 0, 0). The user can
// zoom in/out by using the scroll wheel. The controller does not allow the
// distance to the target (i.e. origin) to be outside of the following interval:
// [min_distance, max_distance]. By default, this is set to [0, 10^9].
// By holding down the right mouse button, the user can orbit around the target.
// The controller imposes no restriction regarding horizontal orbiting. In the
// vertical plane the controller ensures that the user won't go over the head
// nor get too close to the ground plane.
// Also see: http://www.ogre3d.org/tikiwiki/tiki-index.php?page=SdkCameraMan
class GRAPHICS_EXPORT CameraController {
 public:
  CameraController();

  // The camera that will be controlled by this instance. In order to disable
  // (temporarily) the controller, set the camera to NULL.
  Ogre::Camera* camera() const { return camera_; }
  void set_camera(Ogre::Camera* camera) { camera_ = camera; }

  // The controller won't let the user zoom in if the distance to the target
  // will get smaller than |min_distance|.
  double min_distance() const { return min_distance_; }
  void set_min_distance(double min_distance) { min_distance_ = min_distance; }

  // The controller won't let the user zoom out if the distance to the target
  // will get greater than |max_distance|.
  double max_distance() const { return max_distance_; }
  void set_max_distance(double max_distance) { max_distance_ = max_distance; }

  // Can be used to control the speed at which zooming is performed based on
  // mouse scrolling speed. Default value: 1.0.
  double zoom_speed() const { return zoom_speed_; }
  void set_zoom_speed(double zoom_speed) { zoom_speed_ = zoom_speed; }

  // Can be used to control the speed at which orbiting is performed based on
  // mouse movement speed. Default value: 1.0.
  double orbit_speed() const { return orbit_speed_; }
  void set_orbit_speed(double orbit_speed) { orbit_speed_ = orbit_speed; }

  void mouseMoved(const OIS::MouseEvent& event);
  void mousePressed(const OIS::MouseEvent& event, OIS::MouseButtonID id);
  void mouseReleased(const OIS::MouseEvent& event, OIS::MouseButtonID id);

 private:
  Ogre::Camera* camera_;
  double min_distance_;
  double max_distance_;
  double zoom_speed_;
  double orbit_speed_;
  bool orbiting_;

  DISALLOW_COPY_AND_ASSIGN(CameraController);
};

}  // namespace graphics

#endif  // GRAPHICS_CAMERA_CONTROLLER_H_

