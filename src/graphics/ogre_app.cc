// Copyright (c) 2013 Cristian Patrasciuc. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "graphics/ogre_app.h"

#include <stack>
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

#include "OIS/OISEvents.h"
#include "OIS/OISInputManager.h"
#include "OIS/OISKeyboard.h"
#include "OIS/OISMouse.h"

#include "base/log.h"
#include "base/string_util.h"
#include "graphics/game_state.h"

namespace graphics {

OgreApp::OgreApp(const std::string& name) : name_(name) {}

OgreApp::~OgreApp() {
  DCHECK(!input_manager_);
  DCHECK(!keyboard_);
  DCHECK(!mouse_);
}

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

  OIS::ParamList params;
  size_t window_handle = 0;
  render_window_->getCustomAttribute("WINDOW", &window_handle);
  params.insert(std::make_pair("WINDOW", base::ToString(window_handle)));
#if defined(OIS_LINUX_PLATFORM)
  params.insert(std::make_pair("x11_mouse_grab", "false"));
  params.insert(std::make_pair("x11_mouse_hide", "false"));
  params.insert(std::make_pair("x11_keyboard_grab", "false"));
  params.insert(std::make_pair("XAutoRepeatOn", "true"));
#endif
  input_manager_ = OIS::InputManager::createInputSystem(params);
  keyboard_ = static_cast<OIS::Keyboard*>(
      input_manager_->createInputObject(OIS::OISKeyboard, true));
  mouse_ = static_cast<OIS::Mouse*>(
      input_manager_->createInputObject(OIS::OISMouse, true));

  // TODO(OIS): Destroy the OIS system
  windowResized(render_window_);
  Ogre::WindowEventUtilities::addWindowEventListener(render_window_, this);

  return true;
}

void OgreApp::PushState(GameState* state) {
  if (!states_.empty()) {
    states_.top()->Pause();
  }
  states_.push(state);
  states_.top()->Initialize();
}

void OgreApp::PopState() {
  DCHECK(!states_.empty());
  states_.top()->Exit();
  states_.pop();
  if (!states_.empty()) {
    states_.top()->Resume();
  } else {
    root_->queueEndRendering();
  }
}

void OgreApp::RunMainLoop() {
  root_->addFrameListener(this);
  keyboard_->setEventCallback(this);
  mouse_->setEventCallback(this);
  if (!states_.empty()) {
    root_->startRendering();
  }
  while (!states_.empty()) {
    PopState();
  }
  Ogre::WindowEventUtilities::removeWindowEventListener(render_window_, this);
  windowClosed(render_window_);
}

void OgreApp::windowResized(Ogre::RenderWindow* render_window) {
  unsigned int width, height, depth;
  int left, top;
  render_window_->getMetrics(width, height, depth, left, top);
  const OIS::MouseState& mouse_state = mouse_->getMouseState();
  mouse_state.width = width;
  mouse_state.height = height;

  if (!states_.empty()) {
    states_.top()->windowResized(render_window);
  }
}

void OgreApp::windowClosed(Ogre::RenderWindow* render_window) {
  if (render_window == render_window_) {
    if (input_manager_) {
      keyboard_->setEventCallback(NULL);
      mouse_->setEventCallback(NULL);
      input_manager_->destroyInputObject(mouse_);
      input_manager_->destroyInputObject(keyboard_);
      OIS::InputManager::destroyInputSystem(input_manager_);
      input_manager_ = NULL;
      keyboard_ = NULL;
      mouse_ = NULL;
    }
  }
  if (!states_.empty()) {
    states_.top()->windowClosed(render_window);
  }
}

bool OgreApp::frameRenderingQueued(const Ogre::FrameEvent& event) {
  if (render_window_->isClosed()) {
    return false;
  }
  keyboard_->capture();
  mouse_->capture();
  if (!states_.empty()) {
    return states_.top()->frameRenderingQueued(event);
  }
  return false;
}

bool OgreApp::frameStarted(const Ogre::FrameEvent& event) {
  return states_.top()->frameStarted(event);
}

bool OgreApp::frameEnded(const Ogre::FrameEvent& event) {
  return states_.top()->frameEnded(event);
}

bool OgreApp::keyPressed(const OIS::KeyEvent& event) {
  return static_cast<OIS::KeyListener*>(states_.top())->keyPressed(event);
}

bool OgreApp::keyReleased(const OIS::KeyEvent& event) {
  return static_cast<OIS::KeyListener*>(states_.top())->keyReleased(event);
}

bool OgreApp::mouseMoved(const OIS::MouseEvent& event) {
  return static_cast<OIS::MouseListener*>(states_.top())->mouseMoved(event);
}

bool OgreApp::mousePressed(const OIS::MouseEvent& event,
                           OIS::MouseButtonID id) {
  OIS::MouseListener* listener(static_cast<OIS::MouseListener*>(states_.top()));
  return listener->mousePressed(event, id);
}

bool OgreApp::mouseReleased(const OIS::MouseEvent& event,
                            OIS::MouseButtonID id) {
  OIS::MouseListener* listener(static_cast<OIS::MouseListener*>(states_.top()));
  return listener->mouseReleased(event, id);
}

}  // namespace graphics
