// Copyright (c) 2013 Cristian Patrasciuc. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef GRAPHICS_OGRE_APP_H_
#define GRAPHICS_OGRE_APP_H_

#include <stack>
#include <string>

#include "base/basic_macros.h"
#include "base/callable.h"
#include "base/ptr/scoped_ptr.h"
#include "base/singleton.h"
#include "graphics/graphics_export.h"

#include "OGRE/OgreFrameListener.h"
#include "OGRE/OgreWindowEventUtilities.h"
#include "OGRE/OgreWorkQueue.h"

#include "OIS/OISEvents.h"
#include "OIS/OISKeyboard.h"
#include "OIS/OISMouse.h"

namespace base {
class Location;
}

namespace Ogre {
class Camera;
struct FrameEvent;
class RenderWindow;
class Root;
class SceneManager;
}

namespace OIS {
class InputManager;
}

namespace graphics {

class GameState;

class GRAPHICS_EXPORT OgreApp : public base::Singleton<OgreApp, false>,
                                public Ogre::FrameListener,
                                public Ogre::WindowEventListener,
                                public Ogre::WorkQueue::RequestHandler,
                                public Ogre::WorkQueue::ResponseHandler,
                                public OIS::KeyListener,
                                public OIS::MouseListener {
 public:
  const std::string& name() const { return name_; }

  Ogre::SceneManager* scene_manager() { return scene_manager_; }

  Ogre::Camera* camera() { return camera_; }

  Ogre::Root* ogre_root() { return Get(root_); }

  const OIS::Mouse& mouse() const { return *mouse_; }

  Ogre::RenderWindow* window() { return render_window_; }

  bool Init();
  void ShutDown();

  void PushState(GameState* state);
  GameState* PopState();

  void RunMainLoop();

  void PostTaskOnGameLoop(const base::Location& from, base::Closure* task);

 protected:
  OgreApp();
  ~OgreApp();

 private:
  void InitializeResourcePaths();

  // FrameListener overrides
  virtual bool frameStarted(const Ogre::FrameEvent& event);
  virtual bool frameRenderingQueued(const Ogre::FrameEvent& event);
  virtual bool frameEnded(const Ogre::FrameEvent& event);

  // WindowEventListener overrides
  virtual void windowResized(Ogre::RenderWindow* render_window);
  virtual void windowClosed(Ogre::RenderWindow* render_window);

  // Ogre::RequestHandler interface
  virtual Ogre::WorkQueue::Response* handleRequest(
      const Ogre::WorkQueue::Request* request,
      const Ogre::WorkQueue* source_queue);

  // Ogre::ResponseHandler interface
  virtual void handleResponse(const Ogre::WorkQueue::Response* response,
                              const Ogre::WorkQueue* source_queue);

  // KeyListener interface
  virtual bool keyPressed(const OIS::KeyEvent& event);
  virtual bool keyReleased(const OIS::KeyEvent& event);

  // MouseListener interface
  virtual bool mouseMoved(const OIS::MouseEvent& event);
  virtual bool mousePressed(const OIS::MouseEvent& event,
                            OIS::MouseButtonID id);
  virtual bool mouseReleased(const OIS::MouseEvent& event,
                             OIS::MouseButtonID id);

  const std::string name_;
  base::ptr::scoped_ptr<Ogre::Root> root_;
  Ogre::RenderWindow* render_window_;
  Ogre::SceneManager* scene_manager_;
  Ogre::Camera* camera_;

  OIS::InputManager* input_manager_;
  OIS::Keyboard* keyboard_;
  OIS::Mouse* mouse_;

  std::stack<GameState*> states_;

  // Ogre work queue channel used to bounce back tasks on the UI thread.
  Ogre::uint16 channel_;

  friend class base::DefaultCreationPolicy<OgreApp>;
  DISALLOW_COPY_AND_ASSIGN(OgreApp);
};

}  // namespace graphics

#endif  // GRAPHICS_OGRE_APP_H_

