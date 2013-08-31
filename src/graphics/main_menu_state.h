// Copyright (c) 2013 Cristian Patrasciuc. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef GRAPHICS_MAIN_MENU_STATE_H_
#define	GRAPHICS_MAIN_MENU_STATE_H_

#include <string>

#include "base/ptr/scoped_ptr.h"
#include "graphics/graphics_export.h"
#include "graphics/menu_state.h"
#include "graphics/playing_state.h"

namespace graphics {

class GRAPHICS_EXPORT MainMenuState : public MenuState {
 public:
  MainMenuState();

  // MenuState overrides
  virtual void OnMenuOptionSelected(const std::string& option);

 private:
  base::ptr::scoped_ptr<PlayingState> playing_state_;
};

}  // namespace graphics

#endif  // GRAPHICS_MAIN_MENU_STATE_H_

