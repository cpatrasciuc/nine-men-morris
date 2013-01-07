// Copyright (c) 2012 Cristian Patrasciuc. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "console_game/save_game_command_handler.h"

#include <fstream>
#include <string>

#include "base/basic_macros.h"
#include "base/log.h"
#include "base/string_util.h"
#include "game/game.h"
#include "game/game_serializer.h"

namespace console_game {

// static
const char SaveGameCommandHandler::kSaveGameCommandName[] = "save";

SaveGameCommandHandler::SaveGameCommandHandler() {}

SaveGameCommandHandler::~SaveGameCommandHandler() {}

std::string SaveGameCommandHandler::ProcessCommand(const std::string& command,
                                                   game::Game* game_model) {
  const std::string actual_command(
      base::ToLowerCase(base::StripString(command)));
  const size_t command_name_length = arraysize(kSaveGameCommandName) - 1;
  DCHECK_EQ(actual_command.find(kSaveGameCommandName), 0);
  DCHECK_EQ(actual_command[command_name_length], ' ')
      << actual_command[command_name_length];
  const std::string file_name(
      base::StripString(actual_command.substr(command_name_length)));
  std::ofstream out(file_name.c_str());
  game::GameSerializer::SerializeTo(*game_model, &out, false);
  out.close();
  return std::string("Game saved successfully to: ") + file_name;
}

}  // namespace console_game
