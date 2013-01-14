// Copyright (c) 2013 Cristian Patrasciuc. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "console_game/player_actions_command_handler.h"

#include <algorithm>
#include <istream>
#include <iterator>
#include <sstream>
#include <string>
#include <vector>

#include "base/basic_macros.h"
#include "base/log.h"
#include "base/string_util.h"
#include "game/game.h"
#include "game/player_action.h"

namespace {

const char* const kGameCommandNames[] = { "MOVE", "PLACE", "REMOVE" };

const game::PlayerAction::ActionType kActionTypes[] = {
  game::PlayerAction::MOVE_PIECE,
  game::PlayerAction::PLACE_PIECE,
  game::PlayerAction::REMOVE_PIECE
};

std::istream& operator>>(std::istream& in, game::BoardLocation& location) {
  char c = '?';
  int line = -1;
  int column = -1;
  for (int i = 0; i < 2; i++) {
    in.get(c);
    if ('1' <= c && c <= '9') {
      line = c - '1';
    }
    if ('a' <= c && c <= 'z') {
      column = c - 'a';
    }
    if ('A' <= c && c <= 'Z') {
      column = c - 'A';
    }
  }
  location = game::BoardLocation(line, column);
  return in;
}

}  // anonymous namespace

namespace console_game {

PlayerActionsCommandHandler::PlayerActionsCommandHandler() {}

PlayerActionsCommandHandler::~PlayerActionsCommandHandler() {}

std::vector<std::string>
PlayerActionsCommandHandler::SupportedCommandTypes() const {
  std::vector<std::string> result;
  for (size_t i = 0; i < arraysize(kGameCommandNames); ++i) {
    result.push_back(kGameCommandNames[i]);
  }
  return result;
}

bool PlayerActionsCommandHandler::ProcessCommand(
    const std::string& command_type,
    const std::string& args,
    game::Game* game_model) {
  DCHECK(game_model != NULL);

  std::vector<std::string> tokens;
  base::SplitString(args, &tokens);
  if (args.empty()) {
    return false;
  }

  game::PlayerAction::ActionType type(game_model->next_action_type());
  for (size_t i = 0; i < arraysize(kGameCommandNames); ++i) {
    if (base::CompareIgnoreCase(command_type, kGameCommandNames[i]) == 0) {
      type = kActionTypes[i];
      break;
    }
  }

  std::stringstream buffer;
  std::copy(tokens.begin(), tokens.end(),
            std::ostream_iterator<std::string>(buffer));

  game::PlayerAction action(game_model->current_player(), type);
  game::BoardLocation source(-1, -1);
  game::BoardLocation destination(-1, -1);
  switch (action.type()) {
    case game::PlayerAction::MOVE_PIECE:
      buffer >> source >> destination;
      break;
    case game::PlayerAction::PLACE_PIECE:
      buffer >> destination;
      break;
    case game::PlayerAction::REMOVE_PIECE:
      buffer >> source;
      break;
  }
  action.set_source(source);
  action.set_destination(destination);

  if (!game_model->CanExecutePlayerAction(action)) {
    return false;
  }
  game_model->ExecutePlayerAction(action);
  return true;
}

void PlayerActionsCommandHandler::GetHelpMessage(
    const std::string& command_type, std::string* format, std::string* usage) {
  if (command_type == "move") {
    *format = "move <source line><source column> <dest line><dest column>";
    *usage = "Move a piece from source to destination.";
  } else if (command_type == "place") {
    *format = "place <line><column>";
    *usage = "Place a piece at the given position on the board.";
  } else if (command_type == "remove") {
    *format = "remove <line><column>";
    *usage = "Remove the piece at the given position on the board.";
  } else if (command_type == "") {
    *format = "<line1><column1> [<line2><column2>]";
    *usage = "Similar with move/place/remove, but the action type is deduced "
             "from the current game state.";
  } else {
    NOTREACHED();
  }
}

}  // namespace console_game

