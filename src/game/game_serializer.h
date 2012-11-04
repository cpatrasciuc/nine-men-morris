// Copyright (c) 2012 Cristian Patrasciuc. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef GAME_GAME_SERIALIZER_H_
#define GAME_GAME_SERIALIZER_H_

#include <ostream>

#include "game/game_export.h"

namespace game {

class Game;

class GAME_EXPORT GameSerializer {
 public:
  // Serialize the |game| to the given output stream. If |use_binary| is true,
  // the serializer will use a binary format; otherwise it will use a
  // text based output format. If |use_binary| is true, |out| must be a binary
  // stream; otherwise it must be a text based stream.
  static void SerializeTo(const Game& game, std::ostream& out, bool use_binary);
};

}  // namespace game

#endif  // GAME_GAME_SERIALIZER_H_

