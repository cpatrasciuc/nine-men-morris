// Copyright (c) 2013 Cristian Patrasciuc. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef GRAPHICS_GAME_STATE_H_
#define GRAPHICS_GAME_STATE_H_

#include "base/basic_macros.h"
#include "graphics/graphics_export.h"

#include "OGRE/OgreFrameListener.h"
#include "OGRE/OgreWindowEventUtilities.h"

#include "OIS/OISEvents.h"
#include "OIS/OISKeyboard.h"
#include "OIS/OISMouse.h"

namespace graphics {

class OgreApp;

// http://gamedevgeek.com/tutorials/managing-game-states-in-c/
// http://www.ogre3d.org/tikiwiki/tiki-index.php?page=Managing+Game+States+with+OGRE
class GRAPHICS_EXPORT GameState
    : public Ogre::FrameListener,
      public Ogre::WindowEventListener,
      public OIS::KeyListener,
      public OIS::MouseListener {
 public:
  GameState();
  virtual ~GameState();

  OgreApp* app();

  virtual bool Initialize();
  virtual void Exit();

  virtual void Pause();
  virtual void Resume();

 private:
  // KeyListener interface
  virtual bool keyPressed(const OIS::KeyEvent& event);
  virtual bool keyReleased(const OIS::KeyEvent& event);

  // MouseListener interface
  virtual bool mouseMoved(const OIS::MouseEvent& event);
  virtual bool mousePressed(const OIS::MouseEvent& event,
                            OIS::MouseButtonID id);
  virtual bool mouseReleased(const OIS::MouseEvent& event,
                             OIS::MouseButtonID id);

  DISALLOW_COPY_AND_ASSIGN(GameState);
};

}  // namespace graphics

#endif  // GRAPHICS_GAME_STATE_H_

