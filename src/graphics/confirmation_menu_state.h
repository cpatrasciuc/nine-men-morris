// Copyright (c) 2013 Cristian Patrasciuc. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef GRAPHICS_CONFIRMATION_MENU_STATE_H_
#define GRAPHICS_CONFIRMATION_MENU_STATE_H_

#include <string>

#include "graphics/graphics_export.h"
#include "graphics/menu_state.h"

namespace graphics {

// Generic class used to instantiate a confirmation screen that has one question
// and two buttons (Yes and No). It uses the confirmation_menu.overlay script.
class GRAPHICS_EXPORT ConfirmationMenuState : public MenuState {
 public:
  explicit ConfirmationMenuState(const std::string& question);

  // Returns |true| if the user clicked the Yes button.
  bool is_confirmed() const { return confirmed_; }

  // MenuState overrides
  virtual bool Initialize();
  virtual void OnMenuOptionSelected(const std::string& option);

 private:
  const std::string question_;
  bool confirmed_;
};

}  // namespace graphics

#endif  // GRAPHICS_CONFIRMATION_MENU_STATE_H_

