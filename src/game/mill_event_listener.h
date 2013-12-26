// Copyright (c) 2013 Cristian Patrasciuc. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef GAME_MILL_EVENT_LISTENER_H_
#define GAME_MILL_EVENT_LISTENER_H_

#include "game/game_export.h"

namespace game {

class BoardLocation;

class GAME_EXPORT MillEventListener {
 public:
  virtual ~MillEventListener();

  virtual void OnMillEvent(const BoardLocation& location,
                           bool is_part_of_mill) = 0;
};

}  // namespace game

#endif  // GAME_MILL_EVENT_LISTENER_H_

