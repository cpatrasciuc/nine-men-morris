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

// Generic class used to create menu screens.
class GRAPHICS_EXPORT MenuState : public GameState {
 public:
  // When creating a concrete menu screen, the client must provide a concrete
  // delegate implementation that should perform the corresponding actions
  // when a menu option is selected.
  class Delegate {
   public:
    virtual ~Delegate();

    // This method is called when a menu options is selected. The menu option
    // name is provided as argument.
    // NOTE: The current implementation calls this method whenever an overlay
    // element with a non-empty caption is clicked. The delegate should verify
    // if this is indeed a valid menu option.
    virtual void OnMenuOptionSelected(const std::string& option) = 0;

   protected:
    Delegate();

   private:
    DISALLOW_COPY_AND_ASSIGN(Delegate);
  };

  // Instantiates a new menu screen by providing a |menu_name| and a |delegate|.
  // The |menu_name| must be the name of an OGRE overlay available in the
  // resource paths. The |delegate| is responsible for handling user events.
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

