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
  for (int line = 0; line < board.size(); ++line) {
    for (int col = 0; col < board.size(); ++col) {
      const game::BoardLocation source(line, col);
      if (!board.IsValidLocation(source)) {
        continue;
      }
      if (board.GetPieceAt(source) != game_model.current_player()) {
        continue;
      }
      std::vector<game::BoardLocation> neighbours;
      board.GetAdjacentLocations(source, &neighbours);
      for (size_t i = 0; i < neighbours.size(); ++i) {
        if (board.GetPieceAt(neighbours[i]) == game::NO_COLOR) {
          game::PlayerAction action(game_model.current_player(),
                                    game::PlayerAction::MOVE_PIECE);
          action.set_source(source);
          action.set_destination(neighbours[i]);
          actions->push_back(action);
        }
      }
    }
  }
  // TODO(game): Add a game::Game::CanJump() method and use it here.
}

void GetValidPlaceActions(const game::Game& game_model,
                          std::vector<game::PlayerAction>* actions) {
  DCHECK_EQ(game_model.next_action_type(), game::PlayerAction::PLACE_PIECE);
  const game::Board& board = game_model.board();
  for (int line = 0; line < board.size(); ++line) {
    for (int col = 0; col < board.size(); ++col) {
      const game::BoardLocation loc(line, col);
      if (!board.IsValidLocation(loc)) {
        continue;
      }
      if (board.GetPieceAt(loc) != game::NO_COLOR) {
        continue;
      }
      game::PlayerAction action(game_model.current_player(),
                                game::PlayerAction::PLACE_PIECE);
      action.set_destination(loc);
      actions->push_back(action);
    }
  }
}

void GetValidRemoveActions(const game::Game& game_model,
                           std::vector<game::PlayerAction>* actions) {
  DCHECK_EQ(game_model.next_action_type(), game::PlayerAction::REMOVE_PIECE);
  const game::Board& board = game_model.board();
  std::vector<game::BoardLocation> morris_locations;
  bool allow_remove_from_mill = true;
  game::PieceColor opponent = game::GetOpponent(game_model.current_player());
  for (int line = 0; line < board.size(); ++line) {
    for (int col = 0; col < board.size(); ++col) {
      const game::BoardLocation loc(line, col);
      if (!board.IsValidLocation(loc)) {
        continue;
      }
      if (board.GetPieceAt(loc) != opponent) {
        continue;
      }
      if (board.IsPartOfMill(loc)) {
        morris_locations.push_back(loc);
      } else {
        allow_remove_from_mill = false;
        game::PlayerAction action(game_model.current_player(),
                                  game::PlayerAction::REMOVE_PIECE);
        action.set_source(loc);
        actions->push_back(action);
      }
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
