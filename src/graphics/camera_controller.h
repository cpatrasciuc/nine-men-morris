// Copyright (c) 2013 Cristian Patrasciuc. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef GRAPHICS_CAMERA_CONTROLLER_H_
#define GRAPHICS_CAMERA_CONTROLLER_H_

#include "base/basic_macros.h"
#include "graphics/graphics_export.h"

#include "OGRE/OgreCamera.h"

#include "OIS/OISEvents.h"
#include "OIS/OISMouse.h"

namespace graphics {

class GRAPHICS_EXPORT CameraController {
 public:
  explicit CameraController(Ogre::Camera* camera = NULL);

  double min_distance() const { return min_distance_; }
  void set_min_distance(double min_distance) { min_distance_ = min_distance; }
  double max_distance() const { return max_distance_; }
  void set_max_distance(double max_distance) { max_distance_ = max_distance; }

  void mouseMoved(const OIS::MouseEvent& event);
  void mousePressed(const OIS::MouseEvent& event, OIS::MouseButtonID id);
  void mouseReleased(const OIS::MouseEvent& event, OIS::MouseButtonID id);

 private:
  Ogre::Camera* camera_;
  double min_distance_;
  double max_distance_;
  bool orbiting_;

  DISALLOW_COPY_AND_ASSIGN(CameraController);
};

}  // namespace graphics

#endif  // GRAPHICS_CAMERA_CONTROLLER_H_

