// Copyright (c) 2013 Cristian Patrasciuc. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef GRAPHICS_MENU_STATE_H_
#define GRAPHICS_MENU_STATE_H_

#include <memory>
#include <string>

#include "base/basic_macros.h"
#include "graphics/game_state.h"
#include "graphics/graphics_export.h"

#include "OGRE/OgreFrameListener.h"

#include "OIS/OISKeyboard.h"
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
  // The |menu_name| must be the name of an OGRE overlay available in a script
  // in resource paths. The |delegate| is responsible for handling user events.
  MenuState(const std::string& menu_name, std::auto_ptr<Delegate> delegate);

  // The menu option that is selected if the user presses the ESC key. If the
  // delegate does not need to be notified in such a situation, set it to the
  // empty string. The implementation does not verify if |option| is really a
  // valid menu option, so clients can use this to detect the user pressing ESC
  // and handle this event without a depending on an existing menu option.
  // Default value: empty string.
  void set_escape_option(const std::string& option) { escape_option_ = option; }
  const std::string& escape_option() const { return escape_option_; }

  // GameState overrides
  virtual bool Initialize();
  virtual void Exit();
  virtual void Pause();
  virtual void Resume();

 private:
  // KeyListener interface
  virtual bool keyReleased(const OIS::KeyEvent& event);
  // MouseListener overrides
  virtual bool mouseReleased(const OIS::MouseEvent& event,
                             OIS::MouseButtonID id);
  // FrameListener overrides
  virtual bool frameRenderingQueued(const Ogre::FrameEvent& event);

  std::string menu_name_;
  std::auto_ptr<Delegate> delegate_;
  Ogre::Overlay* menu_overlay_;
  std::string escape_option_;
  bool reload_captions_;
};

}  // namespace graphics

#endif  // GRAPHICS_MENU_STATE_H_

