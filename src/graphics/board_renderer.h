// Copyright (c) 2013 Cristian Patrasciuc. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef GRAPHICS_BOARD_RENDERER_H_
#define GRAPHICS_BOARD_RENDERER_H_

#include "base/basic_macros.h"
#include "graphics/graphics_export.h"

#include "OGRE/OgreTexture.h"


namespace game {
class Board;
}

namespace graphics {

class OgreApp;

class GRAPHICS_EXPORT BoardRenderer {
 public:
  explicit BoardRenderer(const game::Board& board);
  ~BoardRenderer();

  void Initialize(OgreApp* app);

 private:
  void GenerateBoardTexture(OgreApp* app);

  const game::Board& board_;
  Ogre::TexturePtr rtt_texture_;

  DISALLOW_COPY_AND_ASSIGN(BoardRenderer);
};

}  // namespace graphics

#endif  // GRAPHICS_BOARD_RENDERER_H_

