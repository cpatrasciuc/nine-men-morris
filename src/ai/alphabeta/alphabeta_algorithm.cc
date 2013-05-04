// Copyright (c) 2013 Cristian Patrasciuc. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "ai/alphabeta/alphabeta_algorithm.h"

#include <memory>
#include <utility>
#include <vector>

#include "ai/ai_algorithm.h"
#include "ai/alphabeta/alphabeta.h"
#include "ai/game_state.h"
#include "ai/game_state_generator.h"
#include "base/basic_macros.h"
#include "base/log.h"
#include "game/board.h"
#include "game/board_location.h"
#include "game/game.h"
#include "game/game_options.h"
#include "game/player_action.h"

namespace ai {
namespace alphabeta {
namespace {

const game::BoardLocation kInvalidLocation(-1, -1);

// Since the AlphaBeta class takes ownership of the delegate, this is needed to
// wrap a pointer to a AlphaBetaAlgorithm when it is passed to an AlphaBeta
// instance, so that it is not deleted when the algorithm completes.
class ProxyPtr : public AlphaBeta<GameState, double>::Delegate {
 public:
  explicit ProxyPtr(AlphaBeta<GameState, double>::Delegate* alg) : alg_(alg) {}

 private:
  // AlphaBeta<GameState, double>::Delegate interface
  virtual bool IsTerminal(const GameState& state) {
    return alg_->IsTerminal(state);
  };

  virtual double Evaluate(const GameState& state, bool max_player) {
    return alg_->Evaluate(state, max_player);
  };

  virtual void GetSuccessors(const GameState& state,
                             std::vector<GameState>* successors) {
    alg_->GetSuccessors(state, successors);
  };

  AlphaBeta<GameState, double>::Delegate* const alg_;

  DISALLOW_COPY_AND_ASSIGN(ProxyPtr);
};

}  // anonymous namespace

AlphaBetaAlgorithm::AlphaBetaAlgorithm(const game::GameOptions& options,
                                       int search_depth,
                                       const std::vector<Evaluator>& evaluators,
                                       const std::vector<double>& weights)
    : options_(options),
      depth_(search_depth),
      evaluators_(evaluators),
      weights_(weights),
      generator_(options),
      remove_location_(kInvalidLocation) {
  DCHECK(!evaluators.empty());
  if (weights_.empty()) {
    weights_.insert(weights_.begin(), evaluators_.size(), 1.0);
  } else {
    DCHECK_EQ(evaluators_.size(), weights_.size());
  }
}

AlphaBetaAlgorithm::~AlphaBetaAlgorithm() {
  for (size_t i = 0; i < evaluators_.size(); ++i) {
    delete evaluators_[i];
    evaluators_[i] = NULL;
  }
}

game::PlayerAction AlphaBetaAlgorithm::GetNextAction(
    const game::Game& game_model) {
  DCHECK_EQ(options_, game_model.options());
  if (game_model.next_action_type() == game::PlayerAction::REMOVE_PIECE) {
    game::PlayerAction action(game_model.current_player(),
                              game::PlayerAction::REMOVE_PIECE);
    action.set_source(remove_location_);
    return action;
  }
  std::auto_ptr<AlphaBeta<GameState, double>::Delegate> delegate(
      new ProxyPtr(this));
  AlphaBeta<GameState, double> alphabeta(delegate);
  GameState origin;
  origin.set_current_player(game_model.current_player());
  origin.set_pieces_in_hand(
      game::WHITE_COLOR, game_model.GetPiecesInHand(game::WHITE_COLOR));
  origin.set_pieces_in_hand(
      game::BLACK_COLOR, game_model.GetPiecesInHand(game::BLACK_COLOR));
  origin.Encode(game_model.board());
  const GameState best_successor = alphabeta.GetBestSuccessor(origin, depth_);
  const std::vector<game::PlayerAction> actions =
      GameState::GetTransition(origin,
                               best_successor,
                               game_model.options().game_type());
  if (actions.size() > 1) {
    remove_location_ = actions[1].source();
  } else {
#if defined(DEBUG_MODE)
    remove_location_ = kInvalidLocation;
#endif
  }
  return actions[0];
}

bool AlphaBetaAlgorithm::IsTerminal(const GameState& state) {
  game::Board board(options_.game_type());
  state.Decode(&board);
  // TODO(alphabeta): Similar logic with game::Game:CheckIfGameIsOver()
  const game::PieceColor opponent = game::GetOpponent(state.current_player());
  const int remaining_pieces_on_board = board.GetPieceCountByColor(opponent);
  const int remaining_pieces_in_hand = state.pieces_in_hand(opponent);
  const int total_remaining_pieces =
      remaining_pieces_on_board + remaining_pieces_in_hand;
  if (total_remaining_pieces <= 2) {
    return true;
  }
  if (remaining_pieces_in_hand > 0) {
    return false;
  }
  if (remaining_pieces_on_board == 3 && options_.jumps_allowed()) {
    // The game is not over and there surely is an empty place where the player
    // can jump to.
    return false;
  }
  // End of similar code.
  std::vector<GameState> successors;
  GetSuccessors(state, &successors);
  return successors.empty();
}

double AlphaBetaAlgorithm::Evaluate(const GameState& state,
                                    bool max_player) {
  game::Board board(options_.game_type());
  state.Decode(&board);
  double result = 0;
  for (size_t i = 0; i < evaluators_.size(); ++i) {
    result += weights_[i] * ((*evaluators_[i])(board, max_player));
  }
  return result;
}

void AlphaBetaAlgorithm::GetSuccessors(const GameState& state,
                                       std::vector<GameState>* successors) {
  SuccessorCache::const_iterator it = successors_cache_.find(state);
  if (it != successors_cache_.end()) {
    successors->insert(successors->end(), it->second.begin(), it->second.end());
    return;
  }
  std::vector<GameState> temp;
  generator_.GetSuccessors(state, &temp);
  successors_cache_.insert(std::make_pair(state, temp));
  successors->insert(successors->end(), temp.begin(), temp.end());
}

}  // namespace alphabeta
}  // namespace ai

