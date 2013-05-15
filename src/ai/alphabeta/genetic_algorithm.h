// Copyright (c) 2013 Cristian Patrasciuc. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef AI_ALPHABETA_GENETIC_ALGORITHM_H_
#define AI_ALPHABETA_GENETIC_ALGORITHM_H_

#include <algorithm>
#include <cstdlib>
#include <ctime>
#include <memory>
#include <vector>

#include "base/basic_macros.h"
#include "base/ptr/scoped_ptr.h"

namespace ai {
namespace alphabeta {

template <class Chromosome>
class GeneticAlgorithm {
 public:
  typedef std::vector<Chromosome> Population;

  class Delegate {
   public:
    virtual ~Delegate() {}

    virtual void GetInitialPopulation(int size, Population* population) {
      population->clear();
      for (int i = 0; i < size; ++i) {
        population->push_back(Chromosome());
      }
    }

    virtual void Tournament(const Population& population) = 0;

    virtual double Fitness(const Chromosome& individual) = 0;

    virtual const Chromosome& Selection(const Population& population) {
      const int index = Rand() * population.size();
      return population[index];
    };

    virtual void Crossover(const Chromosome& c1, const Chromosome& c2,
                           Population* population) = 0;

    virtual void Mutate(Chromosome* individual) = 0;

    virtual void ReportProgress(int generation_count, double best_score) {}

   protected:
    Delegate() {}

   private:
    DISALLOW_COPY_AND_ASSIGN(Delegate)
  };

  explicit GeneticAlgorithm(std::auto_ptr<Delegate> delegate)
      : delegate_(delegate.release()),
        population_(),
        population_size_(100),
        crossover_rate_(0.9),
        mutation_rate_(0.01),
        max_generations_(500),
        current_generation_(0),
        best_(NULL) {
    std::srand(std::time(NULL));
  };

  int population_size() const { return population_size_; }
  void set_population_size(int size) { population_size_ = size; }

  double crossover_rate() const { return crossover_rate_; }
  void set_crossover_rate(double rate) { crossover_rate_ = rate; }

  double mutation_rate() const { return mutation_rate_; }
  void set_mutation_rate(double rate) { mutation_rate_ = rate; }


  int max_generations() const { return max_generations_; }
  void set_max_generations(int count) { max_generations_ = count; }

  void Run() {
    delegate_->GetInitialPopulation(population_size_, &population_);
    while (current_generation_ < max_generations_) {
      NextGeneration();
    }
  }

  Chromosome best() const { return *best_; }

 private:
  class Comparator {
   public:
    explicit Comparator(Delegate* delegate) : delegate_(delegate) {}

    bool operator()(const Chromosome& c1, const Chromosome& c2) {
      return delegate_->Fitness(c1) > delegate_->Fitness(c2);
    }

   private:
    Delegate* const delegate_;
  };

  void NextGeneration() {
    ++current_generation_;
    delegate_->Tournament(population_);
    std::sort(population_.begin(), population_.end(),
        Comparator(Get(delegate_)));
    Population new_population;
    new_population.push_back(population_[0]);
    while (new_population.size() < population_.size()) {
      const Chromosome& mate1 = delegate_->Selection(population_);
      if (Rand() < crossover_rate_) {
        const Chromosome& mate2 = delegate_->Selection(population_);
        delegate_->Crossover(mate1, mate2, &new_population);
      } else {
        new_population.push_back(Chromosome(mate1));
      }
    }
    if (new_population.size() > population_.size()) {
      new_population.pop_back();
    }
    for (int i = 0; i < population_size_; ++i) {
      if (Rand() < mutation_rate_) {
        delegate_->Mutate(&new_population[i]);
      }
    }
    std::swap(population_, new_population);
    best_ = &population_[0];
    delegate_->ReportProgress(current_generation_,
                              delegate_->Fitness(population_[0]));
  }

  static double Rand() {
    return static_cast<double>(std::rand()) / RAND_MAX;  // NOLINT
  }

  base::ptr::scoped_ptr<Delegate> delegate_;
  Population population_;
  int population_size_;
  double crossover_rate_;
  double mutation_rate_;
  int max_generations_;
  int current_generation_;
  Chromosome* best_;

  DISALLOW_COPY_AND_ASSIGN(GeneticAlgorithm);
};

}  // namespace alphabeta
}  // namespace ai

#endif  // AI_ALPHABETA_GENETIC_ALGORITHM_H_

