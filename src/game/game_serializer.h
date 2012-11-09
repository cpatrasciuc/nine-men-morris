// Copyright (c) 2012 Cristian Patrasciuc. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef GAME_GAME_SERIALIZER_H_
#define GAME_GAME_SERIALIZER_H_

#include <memory>
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
  // TODO(serialization): Change |out| from ref to pointer.
  static void SerializeTo(const Game& game, std::ostream& out, bool use_binary);

  // Deserialize a Game instance from the given input stream. If |use_binary| is
  // true, the deserializer will use a binary format and will treat |in| as a
  // binary input stream; otherwise the method will use text deserialization.
  // In case of an error during the deserialization process, the method returns
  // NULL.
  static std::auto_ptr<Game> DeserializeFrom(std::istream* in, bool use_binary);
};

}  // namespace game

#endif  // GAME_GAME_SERIALIZER_H_

