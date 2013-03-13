// Copyright (c) 2013 Cristian Patrasciuc. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "graphics/ogre_app.h"

#include "OGRE/OgreRoot.h"

namespace graphics {

OgreApp::OgreApp() {}

OgreApp::~OgreApp() {}

int OgreApp::RunMainLoop() {
  Ogre::Root* root = new Ogre::Root();
  delete root;
  return 0;
}

}  // namespace graphics
