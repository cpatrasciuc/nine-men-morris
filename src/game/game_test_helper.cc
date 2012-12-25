// Copyright (c) 2012 Cristian Patrasciuc. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include <libgen.h>

#include <cstdio>
#include <fstream>
#include <memory>
#include <sstream>
#include <string>

#include "base/file_path.h"
#include "base/log.h"
#include "game/game.h"
#include "game/game_serializer.h"
#include "game/game_test_helper.h"

namespace game {

const base::FilePath::CharType kSavedGamesFolderName[] =
    FILE_PATH_LITERAL("test_games");
const base::FilePath::CharType kSavedGamesFileExtension[] =
    FILE_PATH_LITERAL(".txt");

const char kStartCommentChar = '#';

base::FilePath GetSavedGamesFolder() {
  base::FilePath current_file_name(FILE_PATH_LITERAL(__FILE__));
  return current_file_name.DirName().Append(kSavedGamesFolderName);
}

std::auto_ptr<Game> LoadSavedGameForTests(const std::string& game_name) {
  base::FilePath file_name(GetSavedGamesFolder()
      .Append(game_name)
      .AddExtension(base::FilePath(kSavedGamesFileExtension)));
  DCHECK(file_name.Exists()) << "File does not exist: " << file_name.value();
  std::ifstream in(file_name.value().c_str());
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
