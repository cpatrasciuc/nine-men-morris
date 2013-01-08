// Copyright (c) 2012 Cristian Patrasciuc. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "console_game/save_game_command_handler.h"

#include <fstream>
#include <string>
#include <vector>

#include "base/basic_macros.h"
#include "base/log.h"
#include "base/string_util.h"
#include "game/game.h"
#include "game/game_serializer.h"

namespace console_game {

namespace {

const char kSaveGameCommandName[] = "save";

}  // anonymous namespace

SaveGameCommandHandler::SaveGameCommandHandler() {}

SaveGameCommandHandler::~SaveGameCommandHandler() {}

std::vector<std::string> SaveGameCommandHandler::SupportedCommandTypes() const {
  std::vector<std::string> result;
  result.push_back(kSaveGameCommandName);
  return result;
}

std::string SaveGameCommandHandler::ProcessCommand(
    const std::string& command_type,
    const std::string& args,
    game::Game* const game_model) {
  const std::string file_name(base::StripString(args));
  std::ofstream out(file_name.c_str());
  game::GameSerializer::SerializeTo(*game_model, &out, false);
  out.close();
  return std::string("Game saved successfully to: ") + file_name;
}

}  // namespace console_game
