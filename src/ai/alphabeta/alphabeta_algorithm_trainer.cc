// Copyright (c) 2013 Cristian Patrasciuc. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifdef ENABLE_DCHECK
#undef ENABLE_DCHECK
#endif

#include <algorithm>
#include <cstdlib>
#include <iostream>
#include <iterator>
#include <map>
#include <memory>
#include <vector>

#include "ai/ai_algorithm.h"
#include "ai/alphabeta/alphabeta_algorithm.h"
#include "ai/alphabeta/evaluators.h"
#include "ai/alphabeta/genetic_algorithm.h"
#include "base/bind.h"
#include "base/callable.h"
#include "base/debug/stacktrace.h"
#include "base/function.h"
#include "base/location.h"
#include "base/log.h"
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

using ai::alphabeta::AlphaBetaAlgorithm;
using ai::alphabeta::Evaluator;
using ai::alphabeta::EvaluatorSignature;
using ai::alphabeta::GeneticAlgorithm;
using base::ptr::scoped_ptr;
using base::threading::ThreadPoolForUnittests;

const int kEvaluatorsCount = 6;
const int kMaxWeight = 5;
const int kMaxMoves = 250;
const int kSearchDepth = 2;

game::GameOptions kGameOptions;
base::threading::Lock kGlobalScoresLock;

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

std::auto_ptr<AlphaBetaAlgorithm> GetPlayer(const Weights& w) {
  std::vector<Evaluator*> evaluators;
  GetEvaluators(&evaluators);
  return std::auto_ptr<AlphaBetaAlgorithm>(
      new AlphaBetaAlgorithm(kGameOptions, kSearchDepth, evaluators, w));
}

void RunGame(const Weights& w1, const Weights& w2, ScoreMap* scores) {
  game::PieceColor winner = game::NO_COLOR;
  game::Game test_game(kGameOptions);
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
    base::threading::ScopedGuard _(&kGlobalScoresLock);
    if (winner == game::WHITE_COLOR) {
      (*scores)[w1] += 3;
    } else if (winner == game::BLACK_COLOR) {
      (*scores)[w2] += 3;
    } else {
      (*scores)[w1] += 1;
      (*scores)[w2] += 1;
    }
  }
  std::cout.put('.');
}

class Trainer : public GeneticAlgorithm<Weights>::Delegate {
 public:
  typedef GeneticAlgorithm<Weights>::Population Population;

  Trainer() {}

  virtual void GetInitialPopulation(int size, Population* population) {
    for (int i = 0; i < size; ++i) {
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
    const double mutation_probability = 0.2;
    for (size_t i = 0; i < weights->size(); ++i) {
      if (base::Random() < mutation_probability) {
        (*weights)[i] = base::Random(2 * kMaxWeight) - kMaxWeight;
      }
    }
  }

  virtual void Process(const Population& population) {
    scores_.clear();
    ThreadPoolForUnittests thread_pool(8);
    thread_pool.CreateThreads();
    thread_pool.StartThreads();
    int thread_idx = 0;
    for (size_t i = 0; i < population.size(); ++i) {
      for (size_t j = 0; j < population.size(); ++j) {
        if (i == j) {
          continue;
        }
        base::Closure* task = base::Bind(
            new base::Function<void(const Weights&, const Weights&, ScoreMap*)>(
                &RunGame),
            population[i],
            population[j],
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

  // TODO(trainer): Implement "natural selection"
  // virtual const Chromosome& Selection(const Population& population);

  virtual void ReportProgress(int gen, double score, const Weights& best) {
    std::cout << std::endl;
    std::cout << "Generation count: " << gen << std::endl;
    std::cout << "Best score: " << static_cast<int>(score) << std::endl;
    std::cout << "Best weights: ";
    std::copy(best.begin(), best.end(),
        std::ostream_iterator<int>(std::cout, " "));
    std::cout << std::endl;
    std::cout << "====================" << std::endl;
  }

 private:
  static void Randomize(Weights* w) {
    for (size_t j = 0; j < w->size(); ++j) {
      (*w)[j] = base::Random(2 * kMaxWeight) - kMaxWeight;
    }
  }

  void AppendUnique(Weights* weights, Population* pop) {
    while (std::find(pop->begin(), pop->end(), *weights) != pop->end()) {
      LOG(INFO) << "Found duplicate individual";
      Randomize(weights);
    }
    pop->push_back(*weights);
  }

  ScoreMap scores_;

  DISALLOW_COPY_AND_ASSIGN(Trainer);
};

}  // anonymous namespace

int main(int argc, char** argv) {
  base::debug::EnableStackTraceDumpOnCrash();
  kGameOptions.set_game_type(game::SIX_MEN_MORRIS);
  std::auto_ptr<GeneticAlgorithm<Weights>::Delegate> delegate;
  delegate.reset(new Trainer());
  GeneticAlgorithm<Weights> alg(delegate);
  alg.set_max_generations(200);
  alg.set_population_size(50);
  alg.set_propagation_rate(0.3);
  const int game_count = alg.max_generations() *
      alg.population_size() * alg.population_size();
  std::cout << "Simulating " << alg.max_generations() << " generations of "
            << alg.population_size() << " individuals, for a total of "
            << game_count << " games." << std::endl;
  alg.Run();
  return EXIT_SUCCESS;
}
