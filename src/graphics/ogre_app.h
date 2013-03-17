// Copyright (c) 2013 Cristian Patrasciuc. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef GRAPHICS_OGRE_APP_H_
#define GRAPHICS_OGRE_APP_H_

#include <string>

#include "base/basic_macros.h"
#include "base/ptr/scoped_ptr.h"
#include "graphics/graphics_export.h"

namespace Ogre {
class Camera;
class RenderWindow;
class Root;
class SceneManager;
}

namespace graphics {

class GRAPHICS_EXPORT OgreApp {
 public:
  explicit OgreApp(const std::string& name);
  ~OgreApp();

  const std::string& name() const { return name_; }

  Ogre::SceneManager* scene_manager() { return scene_manager_; }

  Ogre::Camera* camera() { return camera_; }

  bool Init();

  void RunMainLoop();

 private:
  const std::string name_;
  base::ptr::scoped_ptr<Ogre::Root> root_;
  Ogre::RenderWindow* render_window_;
  Ogre::SceneManager* scene_manager_;
  Ogre::Camera* camera_;

  DISALLOW_COPY_AND_ASSIGN(OgreApp);
};

}  // namespace graphics

#endif  // GRAPHICS_OGRE_APP_H_

