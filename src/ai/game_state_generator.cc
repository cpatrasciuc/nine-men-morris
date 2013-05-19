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

namespace {

void FilterBoardLocations(const game::Board& board,
                          const game::PieceColor player_color,
                          std::vector<game::BoardLocation>* player,
                          std::vector<game::BoardLocation>* removable,
                          std::vector<game::BoardLocation>* empty) {
  std::vector<game::BoardLocation> opponent;
  std::vector<game::BoardLocation> mills;
  const std::vector<game::BoardLocation>& locations = board.ValidLocations();
  for (size_t i = 0; i < locations.size(); ++i) {
    if (!board.IsValidLocation(locations[i])) {
      continue;
    }
    const game::PieceColor loc_color = board.GetPieceAt(locations[i]);
    if (loc_color == game::NO_COLOR) {
      empty->push_back(locations[i]);
    } else if (loc_color == player_color) {
      player->push_back(locations[i]);
    } else if (board.IsPartOfMill(locations[i])) {
      mills.push_back(locations[i]);
    } else {
      opponent.push_back(locations[i]);
    }
  }
  if (opponent.empty()) {
    removable->insert(removable->end(), mills.begin(), mills.end());
  } else {
    removable->insert(removable->end(), opponent.begin(), opponent.end());
  }
}

}  // anonymous namespace

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
  game::Board board(game_options_.game_type());
  state.Decode(&board);
  const game::PieceColor player = state.current_player();
  const game::PieceColor opponent = game::GetOpponent(player);
  std::vector<game::BoardLocation> player_loc;
  std::vector<game::BoardLocation> empty_loc;
  std::vector<game::BoardLocation> opponent_loc;
  FilterBoardLocations(board, player, &player_loc, &opponent_loc, &empty_loc);
  for (size_t i = 0; i < empty_loc.size(); ++i) {
    bool result = board.AddPiece(empty_loc[i], player);
    if (result) {
      if (board.IsPartOfMill(empty_loc[i])) {
        for (size_t k = 0; k < opponent_loc.size(); ++k) {
          board.RemovePiece(opponent_loc[k]);
          GameState successor(state);
          successor.set_current_player(opponent);
          successor.set_pieces_in_hand(player,
                                       state.pieces_in_hand(player) - 1);
          successor.Encode(board);
          successors->push_back(successor);
          board.AddPiece(opponent_loc[k], opponent);
        }
      } else {
        GameState successor(state);
        successor.set_current_player(opponent);
        successor.set_pieces_in_hand(player, state.pieces_in_hand(player) - 1);
        successor.Encode(board);
        successors->push_back(successor);
      }
    } else {
      NOTREACHED();
    }
    result = board.RemovePiece(empty_loc[i]);
    DCHECK(result);
  }
}

void GameStateGenerator::GetMoveSuccessors(const GameState& state,
                                           std::vector<GameState>* successors) {
  game::Board board(game_options_.game_type());
  state.Decode(&board);
  std::vector<game::BoardLocation> player_loc;
  std::vector<game::BoardLocation> empty_loc;
  std::vector<game::BoardLocation> opponent_loc;
  const game::PieceColor player = state.current_player();
  const game::PieceColor opponent = game::GetOpponent(player);
  FilterBoardLocations(board, player, &player_loc, &opponent_loc, &empty_loc);
  for (size_t i = 0; i < player_loc.size(); ++i) {
    for (size_t j = 0; j < empty_loc.size(); ++j) {
      if (!board.IsAdjacent(player_loc[i], empty_loc[j])) {
        if (player_loc.size() > 3 || !game_options_.jumps_allowed()) {
          continue;
        }
      }
      board.MovePiece(player_loc[i], empty_loc[j]);
      if (board.IsPartOfMill(empty_loc[j])) {
        for (size_t k = 0; k < opponent_loc.size(); ++k) {
          board.RemovePiece(opponent_loc[k]);
          GameState successor;
          successor.set_current_player(opponent);
          successor.Encode(board);
          successors->push_back(successor);
          board.AddPiece(opponent_loc[k], opponent);
        }
      } else {
        GameState successor;
        successor.set_current_player(opponent);
        successor.Encode(board);
        successors->push_back(successor);
      }
      board.MovePiece(empty_loc[j], player_loc[i]);
    }
  }
}

}  // namespace ai
