// Copyright (c) 2013 Cristian Patrasciuc. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef GRAPHICS_MENU_STATE_H_
#define GRAPHICS_MENU_STATE_H_

#include <string>

#include "base/basic_macros.h"
#include "graphics/game_state.h"
#include "graphics/graphics_export.h"

#include "OGRE/OgreFrameListener.h"

#include "OIS/OISMouse.h"

namespace Ogre {
class Overlay;
}

namespace graphics {

class GRAPHICS_EXPORT MenuState : public GameState {
 public:
  class Delegate {
   public:
    virtual ~Delegate();

    virtual void OnMenuOptionSelected(const std::string& option) = 0;

   protected:
    Delegate();

   private:
    DISALLOW_COPY_AND_ASSIGN(Delegate);
  };

  MenuState(const std::string& menu_name, Delegate* delegate);

  // GameState overrides
  virtual bool Initialize();
  virtual void Exit();
  virtual void Pause();
  virtual void Resume();

 private:
  // MouseListener overrides
  virtual bool mouseReleased(const OIS::MouseEvent& event,
                             OIS::MouseButtonID id);
  // FrameListener overrides
  virtual bool frameRenderingQueued(const Ogre::FrameEvent& event);

  std::string menu_name_;
  Delegate* delegate_;
  Ogre::Overlay* menu_overlay_;
  bool reload_captions_;
};

}  // namespace graphics

#endif  // GRAPHICS_MENU_STATE_H_

