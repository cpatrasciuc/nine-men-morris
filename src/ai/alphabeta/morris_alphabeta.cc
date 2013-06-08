// Copyright (c) 2013 Cristian Patrasciuc. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "ai/alphabeta/morris_alphabeta.h"

#include <limits>
#include <memory>
#include <utility>
#include <vector>

#include "ai/ai_algorithm.h"
#include "ai/alphabeta/alphabeta.h"
#include "ai/alphabeta/evaluators.h"
#include "ai/game_state.h"
#include "ai/game_state_tree.h"
#include "base/basic_macros.h"
#include "base/bind.h"
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

// TODO(alphabeta): Find the optimal values of these weights using the trainer.
const int kBestWeights[] = { 10, 10, 10, -10, -10, -10 };

const game::BoardLocation kInvalidLocation(-1, -1);

// Since the AlphaBeta class takes ownership of the delegate, this is needed to
// wrap a pointer to a MorrisAlphaBeta when it is passed to an AlphaBeta
// instance, so that it is not deleted when the algorithm completes.
class ProxyPtr : public AlphaBeta<GameState>::Delegate {
 public:
  explicit ProxyPtr(AlphaBeta<GameState>::Delegate* alg) : alg_(alg) {}

 private:
  // AlphaBeta<GameState, double>::Delegate interface
  virtual bool IsTerminal(const GameState& state) {
    return alg_->IsTerminal(state);
  };

  virtual int Evaluate(const GameState& state) {
    return alg_->Evaluate(state);
  };

  virtual void GetSuccessors(const GameState& state,
                             std::vector<GameState>* successors) {
    alg_->GetSuccessors(state, successors);
  };

  AlphaBeta<GameState>::Delegate* const alg_;

  DISALLOW_COPY_AND_ASSIGN(ProxyPtr);
};

}  // anonymous namespace

template <> size_t Hash<GameState>(const GameState& state) {
  return GameState::Hash(state);
}

MorrisAlphaBeta::MorrisAlphaBeta(const game::GameOptions& options)
    : options_(options),
      max_search_depth_(-1),
      max_search_time_(-1),
      tree_(options),
      remove_location_(kInvalidLocation),
      max_player_color_(game::NO_COLOR) {
  typedef int(OppEvalSig)(Evaluator*, const game::Board&, game::PieceColor);
  using base::Function;
  evaluators_.push_back(new Function<EvaluatorSignature>(&Mobility));
  evaluators_.push_back(new Function<EvaluatorSignature>(&Material));
  evaluators_.push_back(new Function<EvaluatorSignature>(&Mills));
  evaluators_.push_back(base::Bind(new Function<OppEvalSig>(&OpponentEval),
      std::auto_ptr<Evaluator>(new Function<EvaluatorSignature>(&Mobility))));
  evaluators_.push_back(base::Bind(new Function<OppEvalSig>(&OpponentEval),
      std::auto_ptr<Evaluator>(new Function<EvaluatorSignature>(&Material))));
  evaluators_.push_back(base::Bind(new Function<OppEvalSig>(&OpponentEval),
      std::auto_ptr<Evaluator>(new Function<EvaluatorSignature>(&Mills))));
  DCHECK_EQ(evaluators_.size(), arraysize(kBestWeights));
  weights_.assign(kBestWeights, kBestWeights + evaluators_.size());
}

MorrisAlphaBeta::MorrisAlphaBeta(const game::GameOptions& options,
    const std::vector<Evaluator*>& evaluators,
    const std::vector<int>& weights)
    : options_(options),
      max_search_depth_(-1),
      max_search_time_(-1),
      evaluators_(evaluators),
      weights_(weights),
      tree_(options),
      remove_location_(kInvalidLocation),
      max_player_color_(game::NO_COLOR) {
  DCHECK(!evaluators.empty());
  if (weights_.empty()) {
    weights_.insert(weights_.begin(), evaluators_.size(), 1.0);
  } else {
    DCHECK_EQ(evaluators_.size(), weights_.size());
  }
}

MorrisAlphaBeta::~MorrisAlphaBeta() {
  for (size_t i = 0; i < evaluators_.size(); ++i) {
    delete evaluators_[i];
    evaluators_[i] = NULL;
  }
}

game::PlayerAction MorrisAlphaBeta::GetNextAction(
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
  if (max_search_depth_ > 0) {
    alphabeta.set_max_search_depth(max_search_depth_);
  }
  if (max_search_time_ > 0) {
    alphabeta.set_max_search_time(max_search_time_);
  }
  GameState origin;
  origin.set_current_player(game_model.current_player());
  origin.set_pieces_in_hand(
      game::WHITE_COLOR, game_model.GetPiecesInHand(game::WHITE_COLOR));
  origin.set_pieces_in_hand(
      game::BLACK_COLOR, game_model.GetPiecesInHand(game::BLACK_COLOR));
  origin.Encode(game_model.board());
  const GameState best_successor = alphabeta.GetBestSuccessor(origin);
  const std::vector<game::PlayerAction> actions =
      GameState::GetTransition(origin, best_successor);
  if (actions.size() > 1) {
    remove_location_ = actions[1].source();
  } else {
#if defined(DEBUG_MODE)
    remove_location_ = kInvalidLocation;
#endif
  }
  return actions[0];
}

bool MorrisAlphaBeta::IsTerminal(const GameState& state) {
  game::Board board(options_.game_type());
  state.Decode(&board);
  // TODO(alphabeta): Similar logic with game::Game:CheckIfGameIsOver()
  const game::PieceColor player = state.current_player();
  const int remaining_pieces_on_board = board.GetPieceCountByColor(player);
  const int remaining_pieces_in_hand = state.pieces_in_hand(player);
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

int MorrisAlphaBeta::Evaluate(const GameState& state) {
  ScoreCache::const_iterator it = score_cache_.find(state);
  if (it != score_cache_.end()) {
    return it->second;
  }
  game::Board board(options_.game_type());
  state.Decode(&board);
  int score = 0;
  for (size_t i = 0; i < evaluators_.size(); ++i) {
    score += weights_[i] * ((*evaluators_[i])(board, max_player_color_));
  }
  score_cache_.insert(std::make_pair(state, score));
  return score;
}

void MorrisAlphaBeta::GetSuccessors(const GameState& state,
                                       std::vector<GameState>* successors) {
  tree_.GetSuccessors(state, successors);
}

}  // namespace alphabeta
}  // namespace ai

