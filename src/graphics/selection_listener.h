// Copyright (c) 2013 Cristian Patrasciuc. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef GRAPHICS_SELECTION_LISTENER_H_
#define GRAPHICS_SELECTION_LISTENER_H_

#include "base/basic_macros.h"
#include "graphics/graphics_export.h"

namespace game {
class BoardLocation;
}

namespace graphics {

class GRAPHICS_EXPORT SelectionListener {
 public:
  virtual ~SelectionListener();

  virtual void OnLocationSelected(const game::BoardLocation& location) = 0;

 protected:
  SelectionListener();

 private:
  DISALLOW_COPY_AND_ASSIGN(SelectionListener);
};

}  // namespace graphics

#endif  // GRAPHICS_SELECTION_LISTENER_H_

