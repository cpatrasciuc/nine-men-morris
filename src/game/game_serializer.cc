// Copyright (c) 2012 Cristian Patrasciuc. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "game/game_serializer.h"

#include <stdint.h>

#include <limits>
#include <memory>
#include <ostream>
#include <string>
#include <vector>

#include "base/log.h"
#include "game/board.h"
#include "game/game.h"
#include "game/game_options.h"
#include "game/player_action.h"

namespace game {

namespace {

const char kMovePieceString[] = "MOVE";
const char kPlacePieceString[] = "PLACE";
const char kRemovePieceString[] = "REMOVE";

const char kWhiteColorString[] = "WHITE";
const char kBlackColorString[] = "BLACK";

std::string ActionTypeToString(const PlayerAction::ActionType type) {
  switch (type) {
    case PlayerAction::MOVE_PIECE:
      return kMovePieceString;
    case PlayerAction::PLACE_PIECE:
      return kPlacePieceString;
    case PlayerAction::REMOVE_PIECE:
      return kRemovePieceString;
  }
  NOTREACHED();
  return std::string();
}

std::string PlayerColorToString(const Board::PieceColor color) {
  switch (color) {
    case Board::WHITE_COLOR:
      return kWhiteColorString;
    case Board::BLACK_COLOR:
      return kBlackColorString;
    case Board::NO_COLOR:
      NOTREACHED();
      break;
  }
  NOTREACHED();
  return std::string();
}

void SerializeActionsToBinaryStream(const std::vector<PlayerAction>& actions,
                                    std::ostream& out) {
  const int64_t count = actions.size();
  out.write(reinterpret_cast<const char*>(&count), sizeof(count));
  for (size_t i = 0; i < actions.size(); ++i) {
    const char type = static_cast<const char>(actions[i].type());
    out.write(&type, sizeof(type));
    const char color = static_cast<const char>(actions[i].player_color());
    out.write(&color, sizeof(color));
    const char action_info[] = {
        actions[i].source().line(),
        actions[i].source().column(),
        actions[i].destination().line(),
        actions[i].destination().column()
    };
    switch (actions[i].type()) {
      case PlayerAction::MOVE_PIECE:
        out.write(action_info, 4 * sizeof(action_info[0]));
        break;
      case PlayerAction::PLACE_PIECE:
        out.write(action_info + 2, 2 * sizeof(action_info[0]));
        break;
      case PlayerAction::REMOVE_PIECE:
        out.write(action_info, 2 * sizeof(action_info[0]));
        break;
    }
  }
}

bool DeserializeActionsFromBinaryStream(std::istream* in,
                                        std::vector<PlayerAction>* actions) {
  if (!in->good()) {
    LOG(ERROR) << "Could not read the number of actions";
    return false;
  }
  int64_t actions_count;
  in->read(reinterpret_cast<char*>(&actions_count), sizeof(actions_count));
  for (int64_t i = 0; i < actions_count; ++i) {
    if (!in->good()) {
      LOG(ERROR) << "Could not read the type for action number " << (i + 1);
      return false;
    }
    char type;
    in->read(&type, sizeof(type));

    if (!in->good()) {
      LOG(ERROR) << "Could not read player color for action number " << (i + 1);
      return false;
    }
    char player_color;
    in->read(&player_color, sizeof(player_color));

    if (!in->good()) {
      LOG(ERROR) << "Could not read the details for action number " << (i + 1);
      return false;
    }
    PlayerAction action(static_cast<Board::PieceColor>(player_color),
                        static_cast<PlayerAction::ActionType>(type));
    char buffer[4];
    switch (type) {
      case PlayerAction::MOVE_PIECE:
        in->read(buffer, 4 * sizeof(buffer[0]));
        action.set_source(BoardLocation(buffer[0], buffer[1]));
        action.set_destination(BoardLocation(buffer[2], buffer[3]));
        break;
      case PlayerAction::PLACE_PIECE:
        in->read(buffer, 2 * sizeof(buffer[0]));
        action.set_destination(BoardLocation(buffer[0], buffer[1]));
        break;
      case PlayerAction::REMOVE_PIECE:
        in->read(buffer, 2 * sizeof(buffer[0]));
        action.set_source(BoardLocation(buffer[0], buffer[1]));
        break;
    }
    actions->push_back(action);
  }
  return true;
}

void SerializeActionsToTextStream(const std::vector<PlayerAction>& actions,
                                    std::ostream& out) {
  out << actions.size() << std::endl;
  for (size_t i = 0; i < actions.size(); ++i) {
    out << ActionTypeToString(actions[i].type()) << " ";
    out << PlayerColorToString(actions[i].player_color()) << " ";
    switch (actions[i].type()) {
      case PlayerAction::MOVE_PIECE:
        out << actions[i].source().line() << " "
            << actions[i].source().column() << " "
            << actions[i].destination().line() << " "
            << actions[i].destination().column();
        break;
      case PlayerAction::PLACE_PIECE:
        out << actions[i].destination().line() << " "
            << actions[i].destination().column();
        break;
      case PlayerAction::REMOVE_PIECE:
        out << actions[i].source().line() << " "
            << actions[i].source().column();
        break;
    }
    out << std::endl;
  }
}

// The encoding is done as follows:
//   - the game type on the first 3 bits
//   - the value of white_starts() on the 4th bit
//   - the value of jumps_allowed() on the 5th bit
int8_t EncodeGameOptions(const GameOptions& options) {
  DCHECK_LT(options.game_type(), 1 << 4);
  int8_t result = static_cast<int8_t>(options.game_type());
  result |= (options.white_starts() ? 1 : 0) << 4;
  result |= (options.jumps_allowed() ? 1 : 0) << 5;
  return result;
}

GameOptions DecodeGameOptions(int8_t encoding) {
  GameOptions options;
  options.set_game_type(static_cast<GameOptions::GameType>(encoding & 0x0F));
  options.set_white_starts(encoding & 0b00010000);
  options.set_jumps_allowed(encoding & 0b00100000);
  return options;
}

}  // anonymous namespace

// static
void GameSerializer::SerializeTo(const Game& game,
                                 std::ostream& out,
                                 bool use_binary) {
  // TODO(serialization): Add version number
  std::vector<PlayerAction> actions;
  game.DumpActionList(&actions);
  int8_t options_encoding = EncodeGameOptions(game.options());
  if (use_binary) {
    DCHECK_EQ(sizeof(char), 1);  // NOLINT(runtime/sizeof)
    DCHECK_LT(game.board().size(), std::numeric_limits<char>().max());
    out.write(reinterpret_cast<const char*>(&options_encoding),
              sizeof(options_encoding));
    SerializeActionsToBinaryStream(actions, out);
  } else {
    out << options_encoding << std::endl;
    SerializeActionsToTextStream(actions, out);
  }
}

// static
std::auto_ptr<Game> GameSerializer::DeserializeFrom(std::istream* in,
                                                    bool use_binary) {
  int8_t options_encoding;
  if (use_binary) {
    in->read(reinterpret_cast<char*>(&options_encoding),
             sizeof(options_encoding));
  } else {
    (*in) >> options_encoding;
  }
  GameOptions options = DecodeGameOptions(options_encoding);
  Game* game = new Game(options);
  game->Initialize();
  std::vector<PlayerAction> actions;
  if (use_binary) {
    if (!DeserializeActionsFromBinaryStream(in, &actions)) {
      LOG(ERROR) << "Could not deserialize game actions";
      return std::auto_ptr<Game>();
    };
  } else {
    // TODO(serialization): Add deserialization for text streams
  }
  SerializeActionsToTextStream(actions, std::cout);
  for (size_t i = 0; i < actions.size(); ++i) {
    if (!game->CanExecutePlayerAction(actions[i])) {
      LOG(ERROR) << "Could not execute action number " << (i + 1);
      return std::auto_ptr<Game>();
    }
    game->ExecutePlayerAction(actions[i]);
  }
  return std::auto_ptr<Game>(game);
}

}  // namespace game
