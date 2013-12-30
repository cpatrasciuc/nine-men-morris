// Copyright (c) 2013 Cristian Patrasciuc. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "graphics/ogre_app.h"

#include <stack>
#include <string>

#include "base/basic_macros.h"
#include "base/callable.h"
#include "base/file_path.h"
#include "base/location.h"
#include "base/log.h"
#include "base/ptr/scoped_ptr.h"
#include "base/string_util.h"
#include "base/threading/task.h"
#include "graphics/game_state.h"

#include "OGRE/OgreAny.h"
#include "OGRE/OgreCamera.h"
#include "OGRE/OgreColourValue.h"
#include "OGRE/OgrePrerequisites.h"
#include "OGRE/OgreRenderWindow.h"
#include "OGRE/OgreResourceGroupManager.h"
#include "OGRE/OgreRoot.h"
#include "OGRE/OgreSceneManager.h"
#include "OGRE/OgreVector3.h"
#include "OGRE/OgreViewport.h"
#include "OGRE/OgreWindowEventUtilities.h"
#include "OGRE/OgreWorkQueue.h"

#include "OIS/OISEvents.h"
#include "OIS/OISInputManager.h"
#include "OIS/OISKeyboard.h"
#include "OIS/OISMouse.h"

namespace graphics {

OgreApp::OgreApp() : name_("Ogre App"), channel_(-1) {}

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

  InitializeResourcePaths();

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

  windowResized(render_window_);
  Ogre::WindowEventUtilities::addWindowEventListener(render_window_, this);

  Ogre::WorkQueue* const work_queue = root_->getWorkQueue();
  channel_ = work_queue->getChannel("Trampoline");
  work_queue->addRequestHandler(channel_, this);
  work_queue->addResponseHandler(channel_, this);

  return true;
}

void OgreApp::ShutDown() {
  Reset(root_);
}

void OgreApp::PushState(GameState* state) {
  if (!states_.empty()) {
    states_.top()->Pause();
  }
  states_.push(state);
  // TODO(game_states): Check the return value of game state methods
  states_.top()->Initialize();
}

GameState* OgreApp::PopState() {
  DCHECK(!states_.empty());
  GameState* const state = states_.top();
  state->Exit();
  states_.pop();
  if (!states_.empty()) {
    states_.top()->Resume();
  } else {
    root_->queueEndRendering();
  }
  return state;
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
  // TODO(OgreApp): What should we do with unprocessed requests?
  Ogre::WorkQueue* const work_queue = root_->getWorkQueue();
  work_queue->removeRequestHandler(channel_, this);
  work_queue->removeResponseHandler(channel_, this);

  Ogre::WindowEventUtilities::removeWindowEventListener(render_window_, this);
  windowClosed(render_window_);
}

void OgreApp::PostTaskOnGameLoop(const base::Location& from,
                                 base::Closure* task) {
  Ogre::WorkQueue* const work_queue = root_->getWorkQueue();
  work_queue->addRequest(channel_, 0,
      Ogre::Any(new base::threading::Task(from, task)));
}

void OgreApp::InitializeResourcePaths() {
  const base::FilePath resource_dir =
      base::FilePath::CurrentDir().Append(FILE_PATH_LITERAL("resources"));
  const base::FilePath::CharType* const sub_dirs[] = {
      FILE_PATH_LITERAL("fonts"),
      FILE_PATH_LITERAL("materials"),
      FILE_PATH_LITERAL("meshes"),
      FILE_PATH_LITERAL("overlay_templates"),
      FILE_PATH_LITERAL("overlays"),
      FILE_PATH_LITERAL("textures"),
      FILE_PATH_LITERAL("particles"),
      FILE_PATH_LITERAL("compositor")
  };

  for (size_t i = 0; i < arraysize(sub_dirs); ++i) {
    Ogre::ResourceGroupManager::getSingleton().addResourceLocation(
        resource_dir.Append(sub_dirs[i]).value(), "FileSystem",
        Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME);
  }

  Ogre::ResourceGroupManager::getSingleton().initialiseAllResourceGroups();
}

void OgreApp::windowResized(Ogre::RenderWindow* render_window) {
  unsigned int width, height, depth;
  int left, top;
  render_window_->getMetrics(width, height, depth, left, top);
  const OIS::MouseState& mouse_state = mouse_->getMouseState();
  mouse_state.width = width;
  mouse_state.height = height;

  camera_->setAspectRatio(static_cast<double>(width) / height);

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
  if (states_.empty()) {
    return false;
  }
  return states_.top()->frameStarted(event);
}

bool OgreApp::frameEnded(const Ogre::FrameEvent& event) {
  if (states_.empty()) {
    return false;
  }
  return states_.top()->frameEnded(event);
}

Ogre::WorkQueue::Response* OgreApp::handleRequest(
    const Ogre::WorkQueue::Request* request,
    const Ogre::WorkQueue* source_queue) {
  return OGRE_NEW Ogre::WorkQueue::Response(request, true, Ogre::Any());
}

void OgreApp::handleResponse(const Ogre::WorkQueue::Response* response,
                             const Ogre::WorkQueue* source_queue) {
  base::ptr::scoped_ptr<base::threading::Task> task(
      response->getRequest()->getData().get<base::threading::Task*>());
  task->Run();
}

bool OgreApp::keyPressed(const OIS::KeyEvent& event) {
  if (states_.empty()) {
    return false;
  }
  return static_cast<OIS::KeyListener*>(states_.top())->keyPressed(event);
}

bool OgreApp::keyReleased(const OIS::KeyEvent& event) {
  if (states_.empty()) {
    return false;
  }
  return static_cast<OIS::KeyListener*>(states_.top())->keyReleased(event);
}

bool OgreApp::mouseMoved(const OIS::MouseEvent& event) {
  if (states_.empty()) {
    return false;
  }
  return static_cast<OIS::MouseListener*>(states_.top())->mouseMoved(event);
}

bool OgreApp::mousePressed(const OIS::MouseEvent& event,
                           OIS::MouseButtonID id) {
  if (states_.empty()) {
    return false;
  }
  OIS::MouseListener* listener(static_cast<OIS::MouseListener*>(states_.top()));
  return listener->mousePressed(event, id);
}

bool OgreApp::mouseReleased(const OIS::MouseEvent& event,
                            OIS::MouseButtonID id) {
  if (states_.empty()) {
    return false;
  }
  OIS::MouseListener* listener(static_cast<OIS::MouseListener*>(states_.top()));
  return listener->mouseReleased(event, id);
}

}  // namespace graphics
