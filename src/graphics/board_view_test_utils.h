// Copyright (c) 2013 Cristian Patrasciuc. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef GRAPHICS_BOARD_VIEW_TEST_UTILS_H_
#define GRAPHICS_BOARD_VIEW_TEST_UTILS_H_

#include "game/piece_color.h"

namespace game {
class BoardLocation;
}

namespace Ogre {
class SceneNode;
class Vector3;
}

namespace graphics {

class BoardView;

// Gets the 3D coordinates corresponding to a BoardLcoation in the 3D space of
// the given BoardView.
const Ogre::Vector3& Get3DPosition(const BoardView& view,
                                   const game::BoardLocation& location);

// Sends a series of mouse events to the |view| instance, in order to simulate
// a user click on the center of the board |location| given as argument.
void ClickOnLocation(BoardView* view, const game::BoardLocation& location);

// Searches for the SceneNode corresponding to the piece belonging to the player
// specified by |color| and being close to the given |position|. The 3D position
// for a given location can be obtained using Get3DPosition(). See above.
// |color| must not be equal to game::NO_COLOR. If a suitable SceneNode cannot
// be found, the method will return NULL.
Ogre::SceneNode* GetPieceByColorAndPosition(game::PieceColor color,
                                            const Ogre::Vector3& position);

}  // namespace graphics

#endif  // GRAPHICS_BOARD_VIEW_TEST_UTILS_H_

