// Copyright (c) 2012 Cristian Patrasciuc. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "game/game.h"

#include <deque>
#include <map>
#include <vector>

#include "base/log.h"
#include "game/game_listener.h"
#include "game/game_options.h"
#include "game/game_type.h"

namespace game {

typedef base::SupportsListener<GameListener>::ListenerList ListenerList;
typedef base::SupportsListener<GameListener>::ListenerIter ListenerIter;

Game::Game(const GameOptions& game_options)
    : game_options_(game_options),
      board_(game_options.game_type()),
      moves_(),
      pieces_in_hand_(),
      current_player_(NO_COLOR),
      next_action_type_(PlayerAction::PLACE_PIECE),
      is_game_over_(false),
      winner_(NO_COLOR) {
}

PieceColor Game::winner() const {
  DCHECK(is_game_over_);
  return winner_;
}

void Game::Initialize() {
  DCHECK(current_player_ == NO_COLOR) << "Game is already initialized";
  int piece_count = GetInitialPieceCountByGameType(game_options_.game_type());
  pieces_in_hand_.insert(std::make_pair(WHITE_COLOR, piece_count));
  pieces_in_hand_.insert(std::make_pair(BLACK_COLOR, piece_count));
  UpdateGameState();
  FireOnGameInitialized();
}

bool Game::CheckIfGameIsOver() const {
  if (is_game_over_) {
    return true;
  }
  const PieceColor opponent = GetOpponent(current_player_);
  const int remaining_pieces_on_board = board_.GetPieceCountByColor(opponent);
  const int remaining_pieces_in_hand = GetPiecesInHand(opponent);
  const int total_remaining_pieces =
      remaining_pieces_on_board + remaining_pieces_in_hand;
  if (total_remaining_pieces <= 2) {
    return true;
  }
  if (remaining_pieces_in_hand > 0) {
    return false;
  }
  if (remaining_pieces_on_board == 3 && game_options_.jumps_allowed()) {
    // The game is not over and there surely is an empty place where the player
    // can jump to.
    return false;
  }
  // TODO(board): Optimize this search if needed
  const std::vector<BoardLocation>& locations = board_.locations();
  for (size_t i = 0; i < locations.size(); ++i) {
    if (board_.GetPieceAt(locations[i]) != opponent) {
      continue;
    }
    std::vector<BoardLocation> adjacent_locations;
    board_.GetAdjacentLocations(locations[i], &adjacent_locations);
    for (size_t k = 0; k < adjacent_locations.size(); ++k) {
      if (board_.GetPieceAt(adjacent_locations[k]) == NO_COLOR) {
        return false;
      }
    }
  }
  return true;
}

bool Game::CanExecutePlayerAction(const PlayerAction& action) const {
  if (is_game_over_) {
    LOG(ERROR) << "Game is over";
    return false;
  }
  if (action.type() != next_action_type_) {
    LOG(ERROR) << "Invalid action type";
    return false;
  }
  if (action.player_color() != current_player_) {
    LOG(ERROR) << "Invalid player color";
    return false;
  }
  if (!action.CanExecuteOn(board_)) {
    LOG(ERROR) << "Cannot execute action on board";
    return false;
  }
  if ((action.type() == PlayerAction::PLACE_PIECE) &&
      (GetPiecesInHand(current_player_) <= 0)) {
    LOG(ERROR) << "No more pieces to be placed";
    return false;
  }
  if ((action.type() == PlayerAction::MOVE_PIECE) &&
      (GetPiecesInHand(current_player_) > 0)) {
    LOG(ERROR) << "There are still pieces to be placed on the board";
    return false;
  }
  return true;
}

void Game::ExecutePlayerAction(const PlayerAction& action) {
  DCHECK(CanExecutePlayerAction(action));
  moves_.push_back(action);
  action.Execute(&board_);
  if (action.type() == PlayerAction::PLACE_PIECE) {
    --pieces_in_hand_[current_player_];
  }
  UpdateGameState();
  FireOnPlayerAction(action);
}

bool Game::CanJump() const {
  DCHECK(current_player_ != game::NO_COLOR);
  if (!game_options_.jumps_allowed()) {
    return false;
  }
  if (next_action_type_ != PlayerAction::MOVE_PIECE) {
    return false;
  }
  if (board_.GetPieceCountByColor(current_player_) > 3) {
    return false;
  }
  return true;
}

void Game::UndoLastAction() {
  if (moves_.empty()) {
    return;
  }
  const PlayerAction last_action = moves_.back();
  moves_.pop_back();
  last_action.Undo(&board_);
  if (last_action.type() == PlayerAction::PLACE_PIECE) {
    ++pieces_in_hand_[last_action.player_color()];
  }
  UpdateGameState();
  FireOnUndoAction(last_action);
}

void Game::UpdateGameState() {
  is_game_over_ = false;
  if (moves_.empty()) {
    next_action_type_ = PlayerAction::PLACE_PIECE;
    if (game_options_.white_starts()) {
      current_player_ = WHITE_COLOR;
    } else {
      current_player_ = BLACK_COLOR;
    }
  } else {
    const PlayerAction action = moves_.back();
    if ((action.type() == PlayerAction::PLACE_PIECE ||
       action.type() == PlayerAction::MOVE_PIECE) &&
       board_.IsPartOfMill(action.destination())) {
      next_action_type_ = PlayerAction::REMOVE_PIECE;
    } else if (CheckIfGameIsOver()) {
        is_game_over_ = true;
        winner_ = current_player_;
        FireOnGameOver(winner_);
    } else {
      current_player_ = GetOpponent(action.player_color());
      next_action_type_ = pieces_in_hand_[current_player_] > 0 ?
                          PlayerAction::PLACE_PIECE :
                          PlayerAction::MOVE_PIECE;
    }
  }
}

void Game::DumpActionList(std::vector<PlayerAction>* actions) const {
  actions->insert(actions->end(), moves_.begin(), moves_.end());
}

int Game::GetPiecesInHand(const PieceColor player_color) const {
  DCHECK(player_color != NO_COLOR);
  return (*pieces_in_hand_.find(player_color)).second;
}

void Game::FireOnGameInitialized() {
  const ListenerList list(listeners());
  for (ListenerIter it = list.begin(); it != list.end(); ++it) {
    (*it)->OnGameInitialized();
  }
}

void Game::FireOnPlayerAction(const PlayerAction& action) {
  const ListenerList list(listeners());
  for (ListenerIter it = list.begin(); it != list.end(); ++it) {
    (*it)->OnPlayerAction(action);
  }
}

void Game::FireOnUndoAction(const PlayerAction& action) {
  const ListenerList list(listeners());
  for (ListenerIter it = list.begin(); it != list.end(); ++it) {
    (*it)->OnUndoPlayerAction(action);
  }
}

void Game::FireOnGameOver(PieceColor winner) {
  const ListenerList list(listeners());
  for (ListenerIter it = list.begin(); it != list.end(); ++it) {
    (*it)->OnGameOver(winner);
  }
}

// static
int Game::GetInitialPieceCountByGameType(GameType type) {
  switch (type) {
    case NINE_MEN_MORRIS:
      return 9;
    case SIX_MEN_MORRIS:
      return 6;
    case THREE_MEN_MORRIS:
      return 3;
  }
  NOTREACHED();
  return -1;
}

}  // namespace game
