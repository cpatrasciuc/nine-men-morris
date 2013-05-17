// Copyright (c) 2013 Cristian Patrasciuc. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "ai/alphabeta/alphabeta_algorithm.h"

#include <limits>
#include <memory>
#include <utility>
#include <vector>

#include "ai/ai_algorithm.h"
#include "ai/alphabeta/alphabeta.h"
#include "ai/alphabeta/evaluators.h"
#include "ai/game_state.h"
#include "ai/game_state_generator.h"
#include "base/basic_macros.h"
#include "base/function.h"
#include "base/log.h"
#include "game/board.h"
#include "game/board_location.h"
#include "game/game.h"
#include "game/game_options.h"
#include "game/piece_color.h"
#include "game/player_action.h"

namespace ai {
namespace alphabeta {
namespace {

const game::BoardLocation kInvalidLocation(-1, -1);

// Since the AlphaBeta class takes ownership of the delegate, this is needed to
// wrap a pointer to a AlphaBetaAlgorithm when it is passed to an AlphaBeta
// instance, so that it is not deleted when the algorithm completes.
class ProxyPtr : public AlphaBeta<GameState>::Delegate {
 public:
  explicit ProxyPtr(AlphaBeta<GameState>::Delegate* alg) : alg_(alg) {}

 private:
  // AlphaBeta<GameState, double>::Delegate interface
  virtual bool IsTerminal(const GameState& state) {
    return alg_->IsTerminal(state);
  };

  virtual int Evaluate(const GameState& state, bool max_player) {
    return alg_->Evaluate(state, max_player);
  };

  virtual void GetSuccessors(const GameState& state,
                             std::vector<GameState>* successors) {
    alg_->GetSuccessors(state, successors);
  };

  AlphaBeta<GameState>::Delegate* const alg_;

  DISALLOW_COPY_AND_ASSIGN(ProxyPtr);
};

}  // anonymous namespace

AlphaBetaAlgorithm::AlphaBetaAlgorithm(const game::GameOptions& options)
    : options_(options),
      depth_(6),
      generator_(options),
      remove_location_(kInvalidLocation),
      max_player_color_(game::NO_COLOR) {
  evaluators_.push_back(new base::Function<EvaluatorSignature>(&Mobility));
  evaluators_.push_back(new base::Function<EvaluatorSignature>(&Material));
  evaluators_.push_back(new base::Function<EvaluatorSignature>(&Mills));
  weights_.insert(weights_.end(), evaluators_.size(), 1.0);
}

AlphaBetaAlgorithm::AlphaBetaAlgorithm(const game::GameOptions& options,
    int search_depth, const std::vector<Evaluator*>& evaluators,
    const std::vector<int>& weights)
    : options_(options),
      depth_(search_depth),
      evaluators_(evaluators),
      weights_(weights),
      generator_(options),
      remove_location_(kInvalidLocation),
      max_player_color_(game::NO_COLOR) {
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
  max_player_color_ = game_model.current_player();
  std::auto_ptr<AlphaBeta<GameState>::Delegate> delegate(new ProxyPtr(this));
  AlphaBeta<GameState> alphabeta(delegate);
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
  const int score = state.current_player() != max_player_color_ ?
    std::numeric_limits<int>::max() : std::numeric_limits<int>::min();
  if (total_remaining_pieces <= 2) {
    score_cache_.insert(std::make_pair(state, score));
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
  if (successors.empty()) {
    score_cache_.insert(std::make_pair(state, score));
  }
  return successors.empty();
}

int AlphaBetaAlgorithm::Evaluate(const GameState& state, bool max_player) {
  ScoreCache::const_iterator it = score_cache_.find(state);
  if (it != score_cache_.end()) {
    return it->second;
  }
  game::Board board(options_.game_type());
  state.Decode(&board);
  int score = 0;
  for (size_t i = 0; i < evaluators_.size(); ++i) {
    score += weights_[i] * ((*evaluators_[i])(board, state.current_player()));
  }
  score_cache_.insert(std::make_pair(state, score));
  return score;
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

