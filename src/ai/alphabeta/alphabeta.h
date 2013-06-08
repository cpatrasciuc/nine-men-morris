// Copyright (c) 2013 Cristian Patrasciuc. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef AI_ALPHABETA_ALPHABETA_H_
#define AI_ALPHABETA_ALPHABETA_H_

#include <time.h>

#include <algorithm>
#include <limits>
#include <memory>
#include <utility>
#include <vector>

#include "base/basic_macros.h"
#include "base/hash_map.h"
#include "base/log.h"
#include "base/ptr/scoped_ptr.h"

namespace ai {
namespace alphabeta {

// Placeholder for hashing function that must be specialized for each concrete
// type that is used to encode a game state.
template <class State> size_t Hash(const State& state);

// This represents a generic implementation of the Alpha Beta Pruning algorithm.
// The template is based on two external classes. The |State| class must be an
// encoding of a game state. It is extensively used throughout the search so
// operations like copying or testing for equality should not take too much
// time. The |Score| class is used to store the result of evaluating the
// states corresponding to the leafs of the partially constructed game tree.
// Requirements for template classes:
//   - The State class must provide a default constructor, copy constructor,
//     assignment operator and == operator.
//   - The Score class must be copyable, assignable and must implement the
//     comparison operators <, == and <=.
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
    // not mandatory to be a terminal one.
    virtual Score Evaluate(const State& state) = 0;

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
      : delegate_(delegate.release()),
        max_search_time_(1000000000),  // One second
        max_search_depth_(std::numeric_limits<int>::max()),
        shuffle_(true) {}

  // Parameter used to limit the time (in nanoseconds) required to perform a
  // search. The value cannot be greater than 2 seconds.
  // The limit is not a strict one. When the time expires the search will still
  // continue until the current depth level is reached. By default the limit is
  // set to one second.
  unsigned int max_search_time() const { return max_search_time_; }
  void set_max_search_time(unsigned int max_time) {
    DCHECK_LT(max_time, 2000000000);  // No more than 2 seconds per search.
    max_search_time_ = max_time;
  }

  // Parameter used to limit the depth of a search. By default there is no
  // limit for this.
  int max_search_depth() const { return max_search_depth_; }
  void set_max_search_depth(int max_depth) { max_search_depth_ = max_depth; }

  // Specify whether the successors of a given state should be shuffled if a
  // re-evaluation is required so that the AI algorithm is not favoring one
  // state if multiple ones have equal evaluations. This introduces some
  // variation between different games against an opponent that tries to do the
  // same sequence of moves. By default, shuffling is enabled.
  bool is_shuffling_enabled() const { return shuffle_; }
  void set_shuffling_enabled(bool enable) { shuffle_ = enable; }

  // Starts an iterative deepening search in the partially constructed game tree
  // that is rooted at the state given by the |origin| argument. It continously
  // increases the depth of the search until |max_search_depth_| is reached or
  // the time required to reach the current depth level exceeded
  // |max_search_time_|. The method returns the best successor state that it
  // found. It is the responsibility of the users of this method to determine
  // the actual move that gets them from |origin| to that specific state.
  State GetBestSuccessor(const State& origin) {
    const Score min_infinity = std::numeric_limits<Score>::min();
    const Score max_infinity = std::numeric_limits<Score>::max();
    clock_gettime(CLOCK_MONOTONIC, &start_time_);
    for (int depth = 1; depth <= max_search_depth_; ++depth) {
      Search(origin, depth, min_infinity, max_infinity, true);
      if (TimedOut()) {
        break;
      }
    }
    typename TranspositionTable::const_iterator it = trans_table_.find(origin);
    DCHECK(it != trans_table_.end());
    DCHECK_GT(it->second.successors.size(), 0);
    return it->second.successors[0];
  };

 private:
  // Enum used to specify how the score of a state stored in the transposition
  // table should be interpreted.
  enum EvalType {
    // The score was obtained by calling |Delegate::Evaluate| on that state.
    EXACT,
    // The exact score is less than or equal to the value from the table.
    ALPHA,
    // The exact score is greater that or equal to the value from the table.
    BETA
  };

  struct TransTableEntry {
    int depth;
    Score score;
    EvalType eval_type;
    std::vector<State> successors;
  };

  struct Hasher {
    size_t operator()(const State& state) const {
      return Hash<State>(state);
    }
  };

  typedef base::hash_map<State,  // NOLINT(build/include_what_you_use)
                         TransTableEntry, Hasher> TranspositionTable;

  bool TimedOut() const {
    const int sec_to_nano = 1000000000;
    timespec now;
    clock_gettime(CLOCK_MONOTONIC, &now);
    timespec diff;
    if (now.tv_nsec < start_time_.tv_nsec) {
      diff.tv_sec = now.tv_sec - start_time_.tv_sec - 1;
      diff.tv_nsec = sec_to_nano + now.tv_nsec - start_time_.tv_nsec;
    } else {
      diff.tv_sec = now.tv_sec - start_time_.tv_sec;
      diff.tv_nsec = now.tv_nsec - start_time_.tv_nsec;
    }
    if (diff.tv_nsec > max_search_time_) {
      return true;
    }
    return (max_search_time_ - diff.tv_nsec) / float(sec_to_nano) < diff.tv_sec;
  }

  // Checks if a state was already evaluated and stored in the transposition
  // table. The method returns |true| if the state was fully evaluated or if it
  // was partially evaluated and does not require further analysis given the
  // current state of the search determined by the |alpha| and |beta| arguments.
  // If the method returns |true| the current score of |state| is filled in the
  // variable pointed by |score|.
  // NOTE: The returned score is not necessarily an exact score.
  bool GetFromTranspositionTable(const TransTableEntry& entry, int depth,
                                 Score alpha, Score beta, Score* score) const {
    if (entry.depth >= depth) {
      if (entry.eval_type == EXACT) {
        *score = entry.score;
        return true;
      }
      if (entry.eval_type == ALPHA && entry.score <= alpha) {
        *score = alpha;
        return true;
      }
      if (entry.eval_type == BETA && entry.score >= beta) {
        *score = beta;
        return true;
      }
    }
    return false;
  }

  // Update the transposition table entry for |state|. If there is no entry for
  // this state, a new one will be created.
  void Update(const State& state, int depth, Score score, EvalType eval_type,
      std::vector<State> successors = std::vector<State>()) {
    TransTableEntry& entry = trans_table_[state];
    entry.depth = depth;
    entry.eval_type = eval_type;
    entry.score = score;
    entry.successors = successors;
  }

  // The core of the Alpha Beta Pruning search algorithm. For more details read
  // http://en.wikipedia.org/wiki/Alpha-beta_pruning.
  Score Search(const State& state, int depth, Score alpha, Score beta,
               bool max_player) {
    typename TranspositionTable::const_iterator it = trans_table_.find(state);
    if (it != trans_table_.end()) {
      Score score;
      if (GetFromTranspositionTable(it->second, depth, alpha, beta, &score)) {
        return score;
      }
    }
    if (depth == 0 || delegate_->IsTerminal(state)) {
      Score score = delegate_->Evaluate(state);
      Update(state, depth, score, EXACT);
      return score;
    }
    std::vector<State> successors;
    if (it != trans_table_.end() && !it->second.successors.empty()) {
      successors = it->second.successors;
    } else {
      delegate_->GetSuccessors(state, &successors);
    }
    if (shuffle_) {
      DCHECK(!successors.empty());
      std::random_shuffle(successors.begin() + 1, successors.end());
    }
    if (max_player) {
      EvalType eval_type = ALPHA;
      for (size_t i = 0; i < successors.size(); ++i) {
        Score s = Search(successors[i], depth - 1, alpha, beta, !max_player);
        if (alpha < s) {
          alpha = s;
          std::swap(successors[i], successors[0]);
          eval_type = EXACT;
        }
        if (beta <= alpha) {
          eval_type = BETA;
          Update(state, depth, beta, BETA, successors);
          break;
        }
      }
      if (eval_type != BETA) {
        Update(state, depth, alpha, eval_type, successors);
      }
      return alpha;
    }
    EvalType eval_type = BETA;
    for (size_t i = 0; i < successors.size(); ++i) {
      Score s = Search(successors[i], depth - 1, alpha, beta, !max_player);
      if (s < beta) {
        beta = s;
        std::swap(successors[i], successors[0]);
        eval_type = EXACT;
      }
      if (beta < alpha || beta == alpha) {
        eval_type = ALPHA;
        Update(state, depth, alpha, ALPHA, successors);
        break;
      }
    }
    if (eval_type != ALPHA) {
      Update(state, depth, beta, eval_type, successors);
    }
    return beta;
  };

  base::ptr::scoped_ptr<Delegate> delegate_;
  unsigned int max_search_time_;
  int max_search_depth_;
  bool shuffle_;
  TranspositionTable trans_table_;
  timespec start_time_;

  DISALLOW_COPY_AND_ASSIGN(AlphaBeta);
};

// Template specializations that explicitly deny the use of |float| and |double|
// as scoring types, since the values returned by std::numeric_limits::min() are
// not OK from the algorithm's point of view.
// See std::numeric_limits::lowest() available in C++11.
// http://en.cppreference.com/w/cpp/types/numeric_limits
template <class State> class AlphaBeta<State, double> {};
template <class State> class AlphaBeta<State, float> {};

}  // namespace alphabeta
}  // namespace ai

#endif  // AI_ALPHABETA_ALPHABETA_H_

