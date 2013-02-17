// Copyright (c) 2013 Cristian Patrasciuc. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include <map>
#include <set>
#include <string>
#include <utility>
#include <vector>

#include "base/log.h"
#include "base/string_util.h"
#include "console_game/command_handler.h"
#include "console_game/console_game.h"
#include "console_game/help_command_handler.h"
#include "console_game/human_player.h"
#include "console_game/player.h"
#include "console_game/player_actions_command_handler.h"
#include "console_game/save_game_command_handler.h"
#include "game/player_action.h"

namespace console_game {

namespace {

const char kDefaultCommandHandlerEntry[] = "";

// TODO(stl): Make this generic and move to base
void SafeInsertToMap(const std::pair<std::string, CommandHandler*>& pair,
                     std::map<std::string, CommandHandler*>* map) {
  std::pair<std::map<std::string, CommandHandler*>::iterator, bool> result(
      map->insert(pair));
  if (!result.second) {
    bool should_delete = true;
    std::map<std::string, CommandHandler*>::const_iterator it;
    for (it = map->begin(); it != map->end(); ++it) {
      if ((it->second == result.first->second) && (it->first != pair.first)) {
        should_delete = false;
        break;
      }
    }
    if (should_delete) {
      delete result.first->second;
    }
    map->erase(result.first);
    result = map->insert(pair);
    DCHECK(result.second);
  }
}

}  // anonymous namespace

HumanPlayer::HumanPlayer(const std::string& name)
    : Player(name),
      command_handlers_() {
  CommandHandler* help_handler(new HelpCommandHandler(command_handlers_));
  SafeInsertToMap(std::make_pair(kDefaultCommandHandlerEntry, help_handler),
                  &command_handlers_);
  RegisterCommandHandler(std::auto_ptr<CommandHandler>(help_handler));
}

HumanPlayer::~HumanPlayer() {
  std::set<CommandHandler*> handlers_to_delete;
  std::map<std::string, CommandHandler*>::iterator it;
  for (it = command_handlers_.begin(); it != command_handlers_.end(); ++it) {
    handlers_to_delete.insert(it->second);
  }
  for (std::set<CommandHandler*>::iterator it = handlers_to_delete.begin();
      it != handlers_to_delete.end(); ++it) {
    delete (*it);
  }
}

void HumanPlayer::RegisterCommandHandler(
    std::auto_ptr<CommandHandler> handler) {
  if (!handler.get()) {
    return;
  }
  CommandHandler* handler_ptr(handler.release());
  std::vector<std::string> command_types(handler_ptr->SupportedCommandTypes());
  for (size_t i = 0; i < command_types.size(); ++i) {
    std::string command_type(
        base::StripString(base::ToLowerCase(command_types[i])));
    if (command_type.empty()) {
      continue;
    }
    DCHECK_EQ(command_type.find_first_of(base::kWhiteSpaceChars),
              std::string::npos) << "No whitespace allow in command types";
    SafeInsertToMap(std::make_pair(command_type, handler_ptr),
                    &command_handlers_);
  }
}

void HumanPlayer::SetupDefaultCommandHandlers() {
  std::auto_ptr<CommandHandler> action_handler(new PlayerActionsCommandHandler);
  SafeInsertToMap(
      std::make_pair(kDefaultCommandHandlerEntry, action_handler.get()),
      &command_handlers_);
  RegisterCommandHandler(action_handler);
  std::auto_ptr<CommandHandler> save_game_handler(new SaveGameCommandHandler);
  RegisterCommandHandler(save_game_handler);
}

std::string HumanPlayer::GetNextAction(game::Game* game_model) {
  const int kMaxCommandSize = 256;
  char cmd_buffer[kMaxCommandSize] = { 0 };

  std::cout << "Next command for " << name() << ": ";
  std::cout.flush();
  if (!std::cin.getline(cmd_buffer, kMaxCommandSize)) {
    current_game()->Quit();
    return std::string();
  };

  std::string command(cmd_buffer);
  if (command.empty()) {
    return "Empty command";
  }
  const size_t pos = command.find_first_of(base::kWhiteSpaceChars);
  std::string command_type(base::ToLowerCase(command.substr(0, pos)));
  if (command_type == "q" || command_type == "quit") {
    current_game()->Quit();
    return std::string();
  }
  std::string args;
  if (pos != std::string::npos) {
    args = command.substr(pos);
  }
  std::map<std::string, CommandHandler*>::iterator it =
      command_handlers_.find(command_type);
  if (it == command_handlers_.end()) {
    command_type = kDefaultCommandHandlerEntry;
    args = command;
    it = command_handlers_.find(kDefaultCommandHandlerEntry);
  }
  if (it != command_handlers_.end()) {
    bool result = it->second->ProcessCommand(command_type, args, game_model);
    if (result) {
      return "Command executed successfully";
    }
  }
  return "Invalid command";
}

}  // namespace console_game
