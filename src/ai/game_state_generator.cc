// Copyright (c) 2013 Cristian Patrasciuc. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "ai/game_state_generator.h"

#include <vector>

#include "ai/game_state.h"
#include "base/log.h"
#include "game/board.h"
#include "game/game_options.h"
#include "game/piece_color.h"

namespace ai {

GameStateGenerator::GameStateGenerator(const game::GameOptions& game_options)
    : game_options_(game_options) {}

void GameStateGenerator::GetSuccessors(const GameState& state,
                                       std::vector<GameState>* successors) {
  if (state.pieces_in_hand(state.current_player()) > 0) {
    GetPlaceSuccessors(state, successors);
  } else {
    GetMoveSuccessors(state, successors);
  }
}

void GameStateGenerator::GetPlaceSuccessors(const GameState& state,
    std::vector<GameState>* successors) {
  // TODO(game_state): Consider place actions that generate a mill
  game::Board board(game_options_.game_type());
  state.Decode(&board);
  const game::PieceColor player = state.current_player();
  const game::PieceColor opponent = game::GetOpponent(player);
  for (int line = 0; line < board.size(); ++line) {
    for (int col = 0; col < board.size(); ++col) {
      const game::BoardLocation loc(line, col);
      if (!board.IsValidLocation(loc)) {
        continue;
      }
      if (board.GetPieceAt(loc) != game::NO_COLOR) {
        continue;
      }
      bool result = board.AddPiece(loc, player);
      if (result) {
        GameState successor;
        successor.set_current_player(opponent);
        successor.set_pieces_in_hand(player, state.pieces_in_hand(player) - 1);
        successor.set_pieces_in_hand(opponent, state.pieces_in_hand(opponent));
        successor.Encode(board);
        successors->push_back(successor);
      } else {
        NOTREACHED();
      }
      result = board.RemovePiece(loc);
      DCHECK(result);
    }
  }
}

void GameStateGenerator::GetMoveSuccessors(const GameState& state,
                                           std::vector<GameState>* successors) {
  game::Board board(game_options_.game_type());
  state.Decode(&board);
  std::vector<game::BoardLocation> player_locations;
  std::vector<game::BoardLocation> empty_locations;
  std::vector<game::BoardLocation> opponent_locations;
  std::vector<game::BoardLocation> morris_locations;
  const game::PieceColor player = state.current_player();
  const game::PieceColor opponent = game::GetOpponent(player);
  for (int line = 0; line < board.size(); ++line) {
    for (int col = 0; col < board.size(); ++col) {
      const game::BoardLocation loc(line, col);
      if (!board.IsValidLocation(loc)) {
        continue;
      }
      const game::PieceColor loc_color = board.GetPieceAt(loc);
      if (loc_color == game::NO_COLOR) {
        empty_locations.push_back(loc);
      } else if (loc_color == player) {
        player_locations.push_back(loc);
      } else if (board.IsPartOfMill(loc)) {
        morris_locations.push_back(loc);
      } else {
        opponent_locations.push_back(loc);
      }
    }
  }
  if (opponent_locations.empty()) {
    opponent_locations = morris_locations;
  }
  for (size_t i = 0; i < player_locations.size(); ++i) {
    for (size_t j = 0; j < empty_locations.size(); ++j) {
      if (!board.IsAdjacent(player_locations[i], empty_locations[j])) {
        if (player_locations.size() > 3 || !game_options_.jumps_allowed()) {
          continue;
        }
      }
      board.MovePiece(player_locations[i], empty_locations[j]);
      if (board.IsPartOfMill(empty_locations[j])) {
        for (size_t k = 0; k < opponent_locations.size(); ++k) {
          board.RemovePiece(opponent_locations[k]);
          GameState successor;
          successor.set_current_player(opponent);
          successor.Encode(board);
          successors->push_back(successor);
          board.AddPiece(opponent_locations[k], opponent);
        }
      } else {
        GameState successor;
        successor.set_current_player(opponent);
        successor.Encode(board);
        successors->push_back(successor);
      }
      board.MovePiece(empty_locations[j], player_locations[i]);
    }
  }
}

}  // namespace ai
