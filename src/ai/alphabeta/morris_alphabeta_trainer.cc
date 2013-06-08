// Copyright (c) 2013 Cristian Patrasciuc. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifdef ENABLE_DCHECK
#undef ENABLE_DCHECK
#endif

#include <algorithm>
#include <cmath>
#include <cstdlib>
#include <iostream>
#include <iterator>
#include <map>
#include <memory>
#include <vector>

#include "ai/ai_algorithm.h"
#include "ai/alphabeta/morris_alphabeta.h"
#include "ai/alphabeta/evaluators.h"
#include "ai/alphabeta/genetic_algorithm.h"
#include "base/bind.h"
#include "base/callable.h"
#include "base/debug/stacktrace.h"
#include "base/function.h"
#include "base/location.h"
#include "base/ptr/scoped_ptr.h"
#include "base/random.h"
#include "base/threading/lock.h"
#include "base/threading/scoped_guard.h"
#include "base/threading/thread_pool_for_unittests.h"
#include "game/game.h"
#include "game/game_options.h"
#include "game/piece_color.h"

namespace {

typedef std::vector<int> Weights;
typedef std::map<Weights, int> ScoreMap;

using ai::alphabeta::MorrisAlphaBeta;
using ai::alphabeta::Evaluator;
using ai::alphabeta::EvaluatorSignature;
using ai::alphabeta::GeneticAlgorithm;
using base::ptr::scoped_ptr;
using base::threading::ThreadPoolForUnittests;

const int kEvaluatorsCount = 6;
const int kMaxWeight = 10;
const int kMaxMoves = 250;
const int kMaxSearchDepth = 25;

game::GameOptions g_game_options;
base::threading::Lock g_scores_lock;

void GetEvaluators(std::vector<Evaluator*>* evaluators) {
  typedef int(OppEvalSig)(Evaluator*, const game::Board&, game::PieceColor);
  using ai::alphabeta::Material;
  using ai::alphabeta::Mills;
  using ai::alphabeta::Mobility;
  using ai::alphabeta::OpponentEval;
  using base::Function;
  evaluators->push_back(new Function<EvaluatorSignature>(&Mobility));
  evaluators->push_back(new Function<EvaluatorSignature>(&Material));
  evaluators->push_back(new Function<EvaluatorSignature>(&Mills));
  evaluators->push_back(base::Bind(new Function<OppEvalSig>(&OpponentEval),
      std::auto_ptr<Evaluator>(new Function<EvaluatorSignature>(&Mobility))));
  evaluators->push_back(base::Bind(new Function<OppEvalSig>(&OpponentEval),
      std::auto_ptr<Evaluator>(new Function<EvaluatorSignature>(&Material))));
  evaluators->push_back(base::Bind(new Function<OppEvalSig>(&OpponentEval),
      std::auto_ptr<Evaluator>(new Function<EvaluatorSignature>(&Mills))));
}

std::auto_ptr<MorrisAlphaBeta> GetPlayer(const Weights& w) {
  std::vector<Evaluator*> evaluators;
  GetEvaluators(&evaluators);
  std::auto_ptr<MorrisAlphaBeta> player(
      new MorrisAlphaBeta(g_game_options, evaluators, w));
  player->set_max_search_depth(kMaxSearchDepth);
  player->set_max_search_time(100000000);
  return player;
}

void RunGame(const Weights& w1, const Weights& w2, ScoreMap* scores) {
  game::PieceColor winner = game::NO_COLOR;
  game::Game test_game(g_game_options);
  scoped_ptr<ai::AIAlgorithm> white(GetPlayer(w1).release());
  scoped_ptr<ai::AIAlgorithm> black(GetPlayer(w2).release());
  test_game.Initialize();
  for (int i = 0; i < kMaxMoves; ++i) {
    ai::AIAlgorithm* next_player =
        Get((test_game.current_player() == game::WHITE_COLOR) ? white : black);
    game::PlayerAction action(next_player->GetNextAction(test_game));
    test_game.ExecutePlayerAction(action);
    if (test_game.is_game_over()) {
      winner = test_game.winner();
      break;
    }
  }
  {
    base::threading::ScopedGuard _(&g_scores_lock);
    if (winner == game::WHITE_COLOR) {
      (*scores)[w1] += 3;
      std::cerr.put('W');
    } else if (winner == game::BLACK_COLOR) {
      (*scores)[w2] += 3;
      std::cerr.put('B');
    } else {
      (*scores)[w1] += 1;
      (*scores)[w2] += 1;
      std::cerr.put('.');
    }
  }
}

class Trainer : public GeneticAlgorithm<Weights>::Delegate {
 public:
  typedef GeneticAlgorithm<Weights>::Population Population;

  Trainer() {}

  virtual void GetInitialPopulation(int size, Population* population) {
    if (size <= 0) {
      return;
    }
    const int best_weights[] = { kMaxWeight, kMaxWeight, kMaxWeight,
                                 -kMaxWeight, -kMaxWeight, -kMaxWeight };
    population->push_back(Weights(best_weights, best_weights + 6));
    for (int i = 1; i < size; ++i) {
      Weights w(kEvaluatorsCount, 0);
      AppendUnique(&w, population);
    }
  }

  virtual void Crossover(const Weights& w1, const Weights& w2, Population* p) {
    Weights offspring1(w1);
    Weights offspring2(w2);
    for (size_t i = 0; i < w1.size(); ++i) {
      if (base::Random() < 0.5) {
        std::swap(offspring1[i], offspring2[i]);
      }
    }
    AppendUnique(&offspring1, p);
    AppendUnique(&offspring2, p);
  }

  virtual void Mutate(Weights* weights) {
    Randomize(weights, 0.5);
  }

  virtual void Process(Population* population) {
    std::cerr << std::endl;
    scores_.clear();
    RemoveDuplicates(population);
    ThreadPoolForUnittests thread_pool(8);
    thread_pool.CreateThreads();
    thread_pool.StartThreads();
    int thread_idx = 0;
    for (size_t i = 0; i < population->size(); ++i) {
      for (size_t j = 0; j < population->size(); ++j) {
        if (i == j) {
          continue;
        }
        base::Closure* task = base::Bind(
            new base::Function<void(const Weights&, const Weights&, ScoreMap*)>(
                &RunGame),
            (*population)[i],
            (*population)[j],
            &scores_);
        thread_pool.SubmitTask(thread_idx, FROM_HERE, task);
        thread_idx = (thread_idx + 1) % thread_pool.thread_count();
      }
    }
    thread_pool.StopAndJoinThreads();
  }

  virtual double Fitness(const Weights& individual) {
    std::map<Weights, int>::const_iterator it = scores_.find(individual);
    if (it == scores_.end()) {
      return 0;
    }
    return it->second;
  }

  // The selection method favors individuals with higher fitness score. For
  // more details regarding how is this achieved/implemented see the comments of
  // ComputeAliasAndCutOffValues().
  virtual const Weights& Selection(const Population& population) {
    if (alias_.empty()) {
      ComputeAliasAndCutOffValues(population.size());
    }
    const double a = base::Random();
    const double b = base::Random();
    int index = int(a * population.size());
    if (b > cutoff_[index]) {
      index = alias_[index];
    }
    return population[index];
  }

  virtual void ReportProgress(int gen, double score, const Weights& best) {
    std::cout << "Generation count: " << gen << std::endl;
    std::cout << "Best score: " << static_cast<int>(score) << std::endl;
    std::cout << "Best weights: ";
    std::copy(best.begin(), best.end(),
        std::ostream_iterator<int>(std::cout, " "));
    std::cout << std::endl;
    std::cout << "====================" << std::endl;
  }

 private:
  static void Randomize(Weights* w, double change_probability = 1.0) {
    for (size_t i = 0; i < w->size(); ++i) {
      if (base::Random() < change_probability) {
        (*w)[i] = base::Random(2 * kMaxWeight) - kMaxWeight;
      }
    }
  }

  void AppendUnique(Weights* weights, Population* pop) {
    while (std::find(pop->begin(), pop->end(), *weights) != pop->end()) {
      Randomize(weights);
    }
    pop->push_back(*weights);
  }

  void RemoveDuplicates(Population* population) {
    for (size_t i = 0; i < population->size(); ++i) {
      const Population::iterator end = population->begin() + i;
      while (std::find(population->begin(), end, (*population)[i]) != end) {
        Mutate(&(*population)[i]);
      }
    }
  }

  // In order to choose randomly an individual from a population given a
  // probability distribution P, we use Walker's algorithm. It requires O(N) set
  // up cost and then it allows to sample in O(1).
  // The probability distribution that we use is proportional with the fitness
  // score, i.e. the first element of the population has the biggest chance of
  // being picked and the last one has the smallest chance of being picked.
  // For a detail description of the algorithm see:
  // http://dl.acm.org/citation.cfm?id=355749
  void ComputeAliasAndCutOffValues(const int population_size) {
    const int n = population_size + 1;
    alias_ = std::vector<int>(n);
    cutoff_ = std::vector<double>(n);
    const int total = ((n + 1) * n) / 2;
    int probability = n;
    std::vector<double> prob_dist(n, 0.0);
    for (int i = 0; i < n; ++i) {
      prob_dist[i] = double(probability) / total;
      --probability;
    }
    std::vector<double> diff(n);
    for (int i = 0; i < n; ++i) {
      alias_[i] = i;
      cutoff_[i] = 0.0;
      diff[i] = prob_dist[i] - 1.0 / n;
    }
    for (int i = 0; i < n; ++i) {
      double min = 0.0;
      double max = 0.0;
      int min_pos = 0;
      int max_pos = 0;
      for (int j = 0; j < n; ++j) {
        if (diff[j] < min) {
          min = diff[j];
          min_pos = j;
        }
        if (diff[j] > max) {
          max = diff[j];
          max_pos = j;
        }
      }
      double sum = 0.0;
      for (int j = 0; j < n; ++j) {
        sum += std::abs(diff[j]);
      }
      if (sum < 0.1e-5) {
        break;
      }
      alias_[min_pos] = max_pos;
      cutoff_[min_pos] = min * n;
      diff[min_pos] = 0.0;
      diff[max_pos] = min + max;
    }
  }

  ScoreMap scores_;

  std::vector<int> alias_;
  std::vector<double> cutoff_;

  DISALLOW_COPY_AND_ASSIGN(Trainer);
};

}  // anonymous namespace

int main(int argc, char** argv) {
  base::debug::EnableStackTraceDumpOnCrash();
  g_game_options.set_game_type(game::THREE_MEN_MORRIS);
  std::auto_ptr<GeneticAlgorithm<Weights>::Delegate> delegate;
  delegate.reset(new Trainer());
  GeneticAlgorithm<Weights> alg(delegate);
  alg.set_max_generations(1);
  alg.set_population_size(50);
  alg.set_propagation_rate(0.2);
  const int game_count = alg.max_generations() *
      alg.population_size() * (alg.population_size() - 1);
  std::cout << "Simulating " << alg.max_generations() << " generations of "
            << alg.population_size() << " individuals, for a total of "
            << game_count << " games." << std::endl;
  alg.Run();
  return EXIT_SUCCESS;
}
