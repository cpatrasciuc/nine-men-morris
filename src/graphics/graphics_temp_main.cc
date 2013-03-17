// Copyright (c) 2013 Cristian Patrasciuc. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "graphics/ogre_app.h"

int main(int argc, char** argv) {
  graphics::OgreApp app("Temp test for OgreApp");
  app.Init();
  app.RunMainLoop();
  return 0;
}
