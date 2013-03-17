// Copyright (c) 2013 Cristian Patrasciuc. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "graphics/ogre_app.h"

#include <string>

#include "OGRE/OgreCamera.h"
#include "OGRE/OgreColourValue.h"
#include "OGRE/OgrePrerequisites.h"
#include "OGRE/OgreRenderWindow.h"
#include "OGRE/OgreRoot.h"
#include "OGRE/OgreSceneManager.h"
#include "OGRE/OgreVector3.h"
#include "OGRE/OgreViewport.h"
#include "OGRE/OgreWindowEventUtilities.h"

namespace graphics {

OgreApp::OgreApp(const std::string& name) : name_(name) {}

OgreApp::~OgreApp() {}

bool OgreApp::Init() {
  // TODO(ogre): Pass the plugin file name?
  Reset(root_, new Ogre::Root());

  // TODO(error_handling): Remove ogre config file if it crashes during startup
  if (!(root_->restoreConfig() || root_->showConfigDialog())) {
    return false;
  }

  render_window_ = root_->initialise(true, name_);
  scene_manager_ = root_->createSceneManager("DefaultSceneManager");

  camera_ = scene_manager_->createCamera("DefaultCamera");
  camera_->setPosition(Ogre::Vector3(0, 0, 80));
  camera_->lookAt(Ogre::Vector3(0, 0, -300));
  camera_->setNearClipDistance(5);

  Ogre::Viewport* const viewport = render_window_->addViewport(camera_);
  viewport->setBackgroundColour(Ogre::ColourValue(0.5f, 0.5f, 0.5f));
  const Ogre::Real width(viewport->getActualWidth());
  const Ogre::Real height(viewport->getActualHeight());
  camera_->setAspectRatio(width / height);

  return true;
}

void OgreApp::RunMainLoop() {
  // TODO(ogre): Replace this with root_->startRendering() and delete #include
  while (true) {
    Ogre::WindowEventUtilities::messagePump();
    if (render_window_->isClosed()) {
      break;
    }
    if (!root_->renderOneFrame()) {
      break;
    }
  }
}

}  // namespace graphics
