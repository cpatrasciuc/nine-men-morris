// Copyright (c) 2013 Cristian Patrasciuc. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef AI_ALPHABETA_ALPHABETA_H_
#define AI_ALPHABETA_ALPHABETA_H_

#include <limits>
#include <map>
#include <memory>
#include <utility>
#include <vector>

#include "base/basic_macros.h"
#include "base/ptr/scoped_ptr.h"

namespace ai {
namespace alphabeta {

// This represents a generic implementation of the Alpha Beta Pruning algorithm.
// The template is based on two external classes. The |State| class must be an
// encoding of a game state. It is extensively used throughout the search so
// operations like copying or testing for equality should not take too much
// time. The |Score| class is used to store the result of evaluating the
// states corresponding to the leafs of the partially constructed game tree.
// Requirements for template classes:
//   - The State class must provide a default constructor, copy constructor,
//     assignment operator and == operator.
//   - The Score class must be copyable, assignable and must provide the < and
//     == operators.
template <class State, class Score = int>
class AlphaBeta {
 public:
  // In order to get the successors of a given state or to evaluate leaf states,
  // the users of this class must provide an implementation of this |Delegate|
  // interface that is tailored to their specific game.
  class Delegate {
   public:
    virtual ~Delegate() {}

    // This method should return |true| if the given |state| is one of the
    // game terminal/ending states.
    virtual bool IsTerminal(const State& state) = 0;

    // This method must return the score of the given |state|. The |state| is
    // not mandatory to be a terminal one. The second argument specifies if the
    // evaluation should be performed from the max_player point of view or not.
    // NOTE: max_player is the player that seeks to maximize the score.
    virtual Score Evaluate(const State& state, bool max_player) = 0;

    // This method must fill in the |successors| vector with the successors of
    // the state given as first argument.
    virtual void GetSuccessors(const State& state,
                               std::vector<State>* successors) = 0;

   protected:
    Delegate() {}

   private:
    DISALLOW_COPY_AND_ASSIGN(Delegate);
  };

  // The constructor receives as argument a Delegate instance that is used to
  // adapt the Alpha Beta Pruning algorithm to a specific game.
  explicit AlphaBeta(std::auto_ptr<Delegate> delegate)
      : delegate_(delegate.release()), best_successor_() {}

  // Starts a search in the partially constructed game tree that is rooted at
  // the state given by the |origin| argument and goes down in the tree for at
  // most |depth| levels. The method returns the best successor state that it
  // found. It is the responsibility of the users of this method to determine
  // the actual move that gets them from |origin| to that specific state.
  State GetBestSuccessor(const State& origin, int depth) {
    best_successor_ = State();
    const Score min_infinity = std::numeric_limits<Score>::min();
    const Score max_infinity = std::numeric_limits<Score>::max();
    Search(origin, depth, min_infinity, max_infinity, true, true);
    return best_successor_;
  };

 private:
  // The core of the Alpha Beta Pruning search algorithm. For more details read
  // http://en.wikipedia.org/wiki/Alpha-beta_pruning.
  Score Search(const State& state, int depth, Score alpha, Score beta,
               bool max_player, bool store_best_successor = false) {
    if (depth == 0 || delegate_->IsTerminal(state)) {
      return delegate_->Evaluate(state, max_player);
    }
    std::vector<State> successors;
    delegate_->GetSuccessors(state, &successors);
    if (max_player) {
      for (size_t i = 0; i < successors.size(); ++i) {
        Score s = Search(successors[i], depth - 1, alpha, beta, !max_player);
        if (alpha < s) {
          alpha = s;
          if (store_best_successor) {
            best_successor_ = successors[i];
          }
        }
        if (beta < alpha || beta == alpha) {
          break;
        }
      }
      return alpha;
    }
    for (size_t i = 0; i < successors.size(); ++i) {
      Score s = Search(successors[i], depth - 1, alpha, beta, !max_player);
      if (s < beta) {
        beta = s;
        if (store_best_successor) {
          best_successor_ = successors[i];
        }
      }
      if (beta < alpha || beta == alpha) {
        break;
      }
    }
    return beta;
  };

  base::ptr::scoped_ptr<Delegate> delegate_;

  // This variable is used to store the temporary best successor during one
  // search (i.e. during one call to |GetBestSuccessor()|).
  State best_successor_;

  DISALLOW_COPY_AND_ASSIGN(AlphaBeta);
};

}  // namespace alphabeta
}  // namespace ai

#endif  // AI_ALPHABETA_ALPHABETA_H_

