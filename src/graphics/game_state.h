// Copyright (c) 2013 Cristian Patrasciuc. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef GRAPHICS_GAME_STATE_H_
#define GRAPHICS_GAME_STATE_H_

#include <OGRE/OgreFrameListener.h>
#include <OGRE/OgreWindowEventUtilities.h>

#include "base/basic_macros.h"
#include "graphics/graphics_export.h"

namespace graphics {

class OgreApp;

// http://gamedevgeek.com/tutorials/managing-game-states-in-c/
// http://www.ogre3d.org/tikiwiki/tiki-index.php?page=Managing+Game+States+with+OGRE
class GRAPHICS_EXPORT GameState
    : public Ogre::FrameListener, public Ogre::WindowEventListener {
 public:
  explicit GameState(OgreApp* app);
  virtual ~GameState();

  OgreApp* app() const { return app_; }

  virtual bool Initialize();
  virtual void Exit();

  virtual void Pause();
  virtual void Resume();

  virtual void Render();

 private:
  OgreApp* const app_;

  DISALLOW_COPY_AND_ASSIGN(GameState);
};

}  // namespace graphics

#endif  // GRAPHICS_GAME_STATE_H_

