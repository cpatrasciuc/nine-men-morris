// Copyright (c) 2012 Cristian Patrasciuc. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "game/game.h"

#include <map>
#include <vector>

#include "base/log.h"
#include "game/game_options.h"

namespace game {

Game::Game(const GameOptions& game_options)
    : game_options_(game_options),
      board_(game_options.game_type()),
      next_action_type_(PlayerAction::PLACE_PIECE),
      is_game_over_(false),
      winner_(Board::NO_COLOR) {
}

Board::PieceColor Game::winner() const {
  DCHECK(is_game_over_);
  return winner_;
}

void Game::Initialize() {
  int piece_count = GetInitialPieceCountByGameType(game_options_.game_type());
  pieces_in_hand_.insert(std::make_pair(Board::WHITE_COLOR, piece_count));
  pieces_in_hand_.insert(std::make_pair(Board::BLACK_COLOR, piece_count));
  UpdateGameState();
}

bool Game::CheckIfGameIsOver() const {
  if (is_game_over_) {
    return true;
  }
  const Board::PieceColor opponent = (current_player_ == Board::WHITE_COLOR) ?
                                     Board::BLACK_COLOR :
                                     Board::WHITE_COLOR;
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
  for (int i = 0; i < board_.size(); ++i) {
    for (int j = 0; j < board_.size(); ++j) {
      const BoardLocation location(i, j);
      if (!board_.IsValidLocation(location)) {
        continue;
      }
      if (board_.GetPieceAt(location) != opponent) {
        continue;
      }
      std::vector<BoardLocation> adjacent_locations;
      board_.GetAdjacentLocations(location, &adjacent_locations);
      for (size_t k = 0; k < adjacent_locations.size(); ++k) {
        if (board_.GetPieceAt(adjacent_locations[k]) == Board::NO_COLOR) {
          return false;
        }
      }
    }
  }
  return true;
}

bool Game::CanExecutePlayerAction(const PlayerAction& action) const {
  return (!is_game_over_) &&
         (action.type() == next_action_type_) &&
         (action.player_color() == current_player_) &&
         action.CanExecuteOn(board_) &&
         ((action.type() != PlayerAction::PLACE_PIECE) ||
          (GetPiecesInHand(current_player_) > 0));
}

void Game::ExecutePlayerAction(const PlayerAction& action) {
  DCHECK(CanExecutePlayerAction(action));
  moves_.push_back(action);
  action.Execute(&board_);
  if (action.type() == PlayerAction::PLACE_PIECE) {
    --pieces_in_hand_[current_player_];
  }
  UpdateGameState();
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
}

void Game::UpdateGameState() {
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
    } else if (CheckIfGameIsOver()) {
        is_game_over_ = true;
        winner_ = current_player_;
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

int Game::GetPiecesInHand(const Board::PieceColor player_color) const {
  DCHECK(player_color != Board::NO_COLOR);
  return (*pieces_in_hand_.find(player_color)).second;
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
