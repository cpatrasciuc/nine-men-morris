// Copyright (c) 2013 Cristian Patrasciuc. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "ai/random/random_algorithm.h"

#include <cstdlib>
#include <ctime>
#include <memory>
#include <vector>

#include "base/log.h"
#include "game/board.h"
#include "game/board_location.h"
#include "game/game.h"
#include "game/piece_color.h"
#include "game/player_action.h"
#include "base/function.h"

namespace ai {
namespace random {

namespace {

void GetValidMoveActions(const game::Game& game_model,
                         std::vector<game::PlayerAction>* actions) {
  DCHECK_EQ(game_model.next_action_type(), game::PlayerAction::MOVE_PIECE);
  const game::Board& board = game_model.board();
  std::vector<game::BoardLocation> piece_locations;
  std::vector<game::BoardLocation> empty_locations;
  const std::vector<game::BoardLocation>& locations = board.locations();
  for (size_t i = 0; i < locations.size(); ++i) {
    const game::PieceColor loc_color = board.GetPieceAt(locations[i]);
    if (loc_color == game::NO_COLOR) {
      empty_locations.push_back(locations[i]);
    } else if (loc_color == game_model.current_player()) {
      piece_locations.push_back(locations[i]);
    }
  }
  for (size_t i = 0; i < piece_locations.size(); ++i) {
    for (size_t j = 0; j < empty_locations.size(); ++j) {
      game::PlayerAction action(game_model.current_player(),
                                game::PlayerAction::MOVE_PIECE);
      action.set_source(piece_locations[i]);
      action.set_destination(empty_locations[j]);
      if ((!game_model.CanJump()) && action.IsJumpOn(board)) {
        continue;
      }
      actions->push_back(action);
    }
  }
}

void GetValidPlaceActions(const game::Game& game_model,
                          std::vector<game::PlayerAction>* actions) {
  DCHECK_EQ(game_model.next_action_type(), game::PlayerAction::PLACE_PIECE);
  const game::Board& board = game_model.board();
  const std::vector<game::BoardLocation>& locations = board.locations();
  for (size_t i = 0; i < locations.size(); ++i) {
    if (board.GetPieceAt(locations[i]) != game::NO_COLOR) {
      continue;
    }
    game::PlayerAction action(game_model.current_player(),
                              game::PlayerAction::PLACE_PIECE);
    action.set_destination(locations[i]);
    actions->push_back(action);
  }
}

void GetValidRemoveActions(const game::Game& game_model,
                           std::vector<game::PlayerAction>* actions) {
  DCHECK_EQ(game_model.next_action_type(), game::PlayerAction::REMOVE_PIECE);
  const game::Board& board = game_model.board();
  std::vector<game::BoardLocation> morris_locations;
  bool allow_remove_from_mill = true;
  game::PieceColor opponent = game::GetOpponent(game_model.current_player());
  const std::vector<game::BoardLocation>& locations = board.locations();
  for (size_t i = 0; i < locations.size(); ++i) {
    if (board.GetPieceAt(locations[i]) != opponent) {
      continue;
    }
    if (board.IsPartOfMill(locations[i])) {
      morris_locations.push_back(locations[i]);
    } else {
      allow_remove_from_mill = false;
      game::PlayerAction action(game_model.current_player(),
                                game::PlayerAction::REMOVE_PIECE);
      action.set_source(locations[i]);
      actions->push_back(action);
    }
  }
  if (allow_remove_from_mill) {
    for (size_t i = 0; i < morris_locations.size(); ++i) {
      game::PlayerAction action(game_model.current_player(),
                                game::PlayerAction::REMOVE_PIECE);
      action.set_source(morris_locations[i]);
      actions->push_back(action);
    }
  }
}

}  // anonymous namespace

RandomAlgorithm::RandomAlgorithm()
    : random_number_generator_(new base::Function<int(void)>(&std::rand)) {
  std::srand(std::time(NULL));
}

RandomAlgorithm::RandomAlgorithm(std::auto_ptr<RandomNumberGenerator> random)
    : random_number_generator_(random.release()) {
}

game::PlayerAction RandomAlgorithm::GetNextAction(const game::Game& model) {
  std::vector<game::PlayerAction> valid_actions;
  switch (model.next_action_type()) {
    case game::PlayerAction::MOVE_PIECE:
      GetValidMoveActions(model, &valid_actions);
      break;
    case game::PlayerAction::PLACE_PIECE:
      GetValidPlaceActions(model, &valid_actions);
      break;
    case game::PlayerAction::REMOVE_PIECE:
      GetValidRemoveActions(model, &valid_actions);
      break;
  }
  DCHECK_GT(valid_actions.size(), 0);
  int index = (*random_number_generator_)() % valid_actions.size();
  const game::PlayerAction& next_action = valid_actions[index];
  DCHECK(model.CanExecutePlayerAction(next_action));
  return next_action;
}

}  // namespace random
}  // namespace ai
