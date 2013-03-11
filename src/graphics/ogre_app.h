// Copyright (c) 2013 Cristian Patrasciuc. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef GRAPHICS_OGRE_APP_H_
#define GRAPHICS_OGRE_APP_H_

#include "base/basic_macros.h"
#include "graphics/graphics_export.h"

namespace graphics {

class GRAPHICS_EXPORT OgreApp {
 public:
  OgreApp();
  ~OgreApp();

  int RunMainLoop();

 private:
  DISALLOW_COPY_AND_ASSIGN(OgreApp);
};

}  // namespace graphics

#endif  // GRAPHICS_OGRE_APP_H_

