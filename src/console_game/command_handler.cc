// Copyright (c) 2012 Cristian Patrasciuc. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "console_game/command_handler.h"

#include <istream>
#include <sstream>
#include <string>
#include <vector>
#include <glob.h>

#include "base/log.h"
#include "base/string_util.h"
#include "game/game.h"
#include "game/player_action.h"

namespace {

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

CommandHandler::CommandHandler() {}

CommandHandler::~CommandHandler() {}

DefaultCommandHandler::DefaultCommandHandler() {}

DefaultCommandHandler::~DefaultCommandHandler() {}

std::string DefaultCommandHandler::ProcessCommand(const std::string& command,
                                                  game::Game* game_model) {
  DCHECK(game_model != NULL);

  std::vector<std::string> tokens;
  base::SplitString(command, &tokens);
  if (tokens.empty()) {
    return "Empty command";
  }

  game::PlayerAction::ActionType type(game_model->next_action_type());
  // TODO(string_util): Add case insensitive string compare
  if (tokens[0] == "MOVE") {
    type = game::PlayerAction::MOVE_PIECE;
    tokens.erase(tokens.begin());
  } else if (tokens[0] == "PLACE") {
    type = game::PlayerAction::PLACE_PIECE;
    tokens.erase(tokens.begin());
  } else if (tokens[0] == "REMOVE") {
    type = game::PlayerAction::REMOVE_PIECE;
    tokens.erase(tokens.begin());
  }

  // TODO(STL): Use the copy() function and a back inserter (?)
  std::stringstream buffer;
  for (size_t i = 0; i < tokens.size(); ++i) {
    buffer << tokens[i];
  }

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
    return "Invalid player action";
  }
  game_model->ExecutePlayerAction(action);
  return "Command executed successfully";
}

}  // namespace console_game
