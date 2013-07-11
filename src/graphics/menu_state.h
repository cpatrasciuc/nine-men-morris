// Copyright (c) 2013 Cristian Patrasciuc. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef GRAPHICS_MENU_STATE_H_
#define GRAPHICS_MENU_STATE_H_

#include <string>

#include "graphics/game_state.h"
#include "graphics/graphics_export.h"

namespace Ogre {
class Overlay;
}

namespace graphics {

class GRAPHICS_EXPORT MenuState : public GameState {
 public:
  explicit MenuState(const std::string& menu_name);

  // GameState overrides
  virtual bool Initialize();
  virtual void Exit();
  virtual void Pause();
  virtual void Resume();

 private:
  std::string menu_name_;
  Ogre::Overlay* menu_overlay_;
};

}  // namespace graphics

#endif  // GRAPHICS_MENU_STATE_H_

