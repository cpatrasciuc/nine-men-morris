// Copyright (c) 2012 Cristian Patrasciuc. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "game/game.h"

#include <map>
#include <vector>

#include "base/log.h"
#include "base/ptr/scoped_ptr.h"
#include "game/game_options.h"
#include "game/player.h"

using base::ptr::scoped_ptr;

namespace game {

namespace {

int GetBoardSizeFromGameType(GameOptions::GameType type) {
  switch (type) {
    case GameOptions::THREE_MEN_MORRIS:
      return 3;
    case GameOptions::SIX_MEN_MORRIS:
      return 5;
    case GameOptions::NINE_MEN_MORRIS:
      return 7;
  }
  NOTREACHED();
  return -1;
}

}  // anonymous namespace

Game::Game(const GameOptions& game_options,
           Player* white_player,
           Player* black_player)
    : game_options_(game_options),
      white_player_(white_player),
      black_player_(black_player),
      board_(GetBoardSizeFromGameType(game_options.game_type())),
      next_action_type_(PlayerAction::PLACE_PIECE) {
  white_player->set_game(this);
  black_player->set_game(this);
  pieces_in_hand_.insert(std::make_pair(Board::WHITE_COLOR, 0));
  pieces_in_hand_.insert(std::make_pair(Board::BLACK_COLOR, 0));
}

void Game::Initialize() {
  int piece_count = GetInitialPieceCountByGameType(game_options_.game_type());
  pieces_in_hand_[Board::WHITE_COLOR] = piece_count;
  pieces_in_hand_[Board::BLACK_COLOR] = piece_count;
  UpdateCurrentPlayerAndActionType();
}

bool Game::IsGameOver() const {
  int remaining_pieces = board_.GetPieceCountByColor(current_player_);
  if (remaining_pieces < 2) {
    return true;
  }
  if (remaining_pieces == 3 && game_options_.jumps_allowed()) {
    // The game is not over and there surely is an empty place where the player
    // can jump to.
    return false;
  }
  // TODO(board): Optimize this search if needed
  for (int i = 0; i < board_.size(); ++i) {
    for (int j = 0; j < board_.size(); ++j) {
      const BoardLocation location(i, j);
      if (board_.GetPieceAt(location) != current_player_) {
        continue;
      }
      std::vector<BoardLocation> adjacent_locations;
      board_.GetAdjacentLocations(location, &adjacent_locations);
      for (size_t k = 0; k < adjacent_locations.size(); ++k) {
        if (board_.GetPieceAt(adjacent_locations[k]) == Board::NO_COLOR) {
          return true;
        }
      }
    }
  }
  return false;
}

void Game::ExecutePlayerAction(const PlayerAction& action) {
  DCHECK_EQ(next_action_type_, action.type());
  moves_.push_back(action);
  action.Execute(&board_);
  if (action.type() == PlayerAction::PLACE_PIECE) {
    DCHECK_GT(pieces_in_hand_[current_player_], 0);
    --pieces_in_hand_[current_player_];
  }
  UpdateCurrentPlayerAndActionType();
}

void Game::UndoLastAction() {
  if (moves_.empty()) {
    return;
  }
  PlayerAction last_action = moves_.back();
  moves_.pop_back();
  last_action.Undo(&board_);
  if (last_action.type() == PlayerAction::PLACE_PIECE) {
    ++pieces_in_hand_[last_action.player_color()];
  }
  UpdateCurrentPlayerAndActionType();
}

Player* Game::GetCurrentPlayer() const {
  return Get((current_player_ == Board::WHITE_COLOR) ?
              white_player_ :
              black_player_);
}

void Game::UpdateCurrentPlayerAndActionType() {
  if (moves_.empty()) {
    next_action_type_ = PlayerAction::PLACE_PIECE;
    if (game_options_.white_starts()) {
      current_player_ = Board::WHITE_COLOR;
    } else {
      current_player_ = Board::BLACK_COLOR;
    }
  } else {
    const PlayerAction action = moves_.back();
    if ((action.type() == PlayerAction::PLACE_PIECE ||
       action.type() == PlayerAction::MOVE_PIECE) &&
       board_.IsPartOfMill(action.destination())) {
      next_action_type_ = PlayerAction::REMOVE_PIECE;
    } else {
      current_player_ = action.player_color() == Board::WHITE_COLOR ?
                        Board::BLACK_COLOR :
                        Board::WHITE_COLOR;
      next_action_type_ = pieces_in_hand_[current_player_] > 0 ?
                          PlayerAction::PLACE_PIECE :
                          PlayerAction::MOVE_PIECE;
    }
  }
}

void Game::DumpActionList(std::vector<PlayerAction>* actions) const {
  actions->insert(actions->end(), moves_.begin(), moves_.end());
}

// static
int Game::GetInitialPieceCountByGameType(GameOptions::GameType type) {
  switch (type) {
    case GameOptions::NINE_MEN_MORRIS:
      return 9;
    case GameOptions::SIX_MEN_MORRIS:
      return 6;
    case GameOptions::THREE_MEN_MORRIS:
      return 3;
  }
  NOTREACHED();
  return -1;
}

}  // namespace game
