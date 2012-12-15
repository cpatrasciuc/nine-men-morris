// Copyright (c) 2012 Cristian Patrasciuc. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include <libgen.h>

#include <cstdio>
#include <fstream>
#include <memory>
#include <sstream>
#include <string>

#include "base/log.h"
#include "game/game.h"
#include "game/game_serializer.h"
#include "game/game_test_helper.h"

namespace game {

const char kSavedGamesFolderName[] = "test_games";
const char kSavedGamesFileExtension[] = ".txt";
const char kStartCommentChar = '#';

std::auto_ptr<Game> LoadSavedGameForTests(const std::string& game_name) {
  // TODO(file_util): Compose path
  const int kMaxFileNameLength = 256;
  char current_file_name[kMaxFileNameLength];
  snprintf(current_file_name, kMaxFileNameLength, "%s", __FILE__);
  std::string file_name(dirname(current_file_name));
  file_name += "/";  // TODO(file_util): Get OS independent file separator
  file_name += kSavedGamesFolderName;
  file_name += "/";
  file_name += game_name;
  file_name += kSavedGamesFileExtension;
  std::ifstream in(file_name.c_str());
  // TODO(file_util): Check if a file exists
  DCHECK(in.good()) << "File does not exist: " << file_name;
  std::stringstream contents;
  const int kMaxBufSize = 256;
  char buf[kMaxBufSize];
  while (in.good()) {
    in.getline(buf, kMaxBufSize);
    if (buf[0] != kStartCommentChar) {
      contents << buf << std::endl;
    }
  }
  return GameSerializer::DeserializeFrom(&contents, false);
}

}  // namespace game
