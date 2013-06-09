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
#include "base/log.h"
#include "base/ptr/scoped_ptr.h"
#include "base/random.h"

namespace ai {
namespace alphabeta {

// This class implements a generic genetic algorithm. Each individual in the
// population is an instance of the Chromosome class. The Chromosome class must
// have value semantics (i.e. it must be copyable and assignable).
// See: http://code.activestate.com/recipes/199121-a-simple-genetic-algorithm/
template <class Chromosome>
class GeneticAlgorithm {
 public:
  typedef std::vector<Chromosome> Population;

  // Users of the generic GeneticAlgorithm class must extend/implement the
  // Delegate class to customize the behavior for specific problems.
  class Delegate {
   public:
    virtual ~Delegate() {}

    // This method is responsible for generating the initial population. It must
    // add |size| members to the |population| argument. The default
    // implementation relies on the fact that the default constructor of the
    // Chromosome class initializes a random individual.
    virtual void GetInitialPopulation(int size, Population* population) {
      for (int i = 0; i < size; ++i) {
        population->push_back(Chromosome());
      }
    }

    // This method can be overridden if the user wants to perform additional
    // actions with the members of a population in-between generations.
    virtual void Process(Population* population) {}

    // This method must return the fitness score of one individual.
    virtual double Fitness(const Chromosome& individual) = 0;

    // This method must implement the desired selection algorithm. The default
    // implementation randomly chooses one individual. More advance
    // implementations might simulate the natural selection algorithm, where
    // the selection probability is proportional with the fitness score.
    virtual const Chromosome& Selection(const Population& population) {
      const int index = base::Random(population.size());
      return population[index];
    };

    // This method must implement the crossover algorithm. The resulting
    // offspring(s) must be added to |population|.
    virtual void Crossover(const Chromosome& c1, const Chromosome& c2,
                           Population* population) = 0;

    // This method must perform a mutation on |individual|.
    virtual void Mutate(Chromosome* individual) = 0;

    // This method must be overridden if the user wants to track the progress of
    // the algorithm. The GeneticAlgorithm class will call back this method
    // during its run and provides the current generation count, the current
    // best fitness score and the best found individual after each generation.
    virtual void ReportProgress(int generation_count,
                                double best_score,
                                const Chromosome& best_individual) {}

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
        propagation_rate_(0.0),
        max_generations_(500),
        current_generation_(0),
        best_(NULL) {
    std::srand(std::time(NULL));
  };

  // The number of individuals in the population.
  int population_size() const { return population_size_; }
  void set_population_size(int size) { population_size_ = size; }

  // A number between 0.0 and 1.0 specifying the chances of a selected
  // individual to perform a crossover against simply moving to the next
  // generation. 0.0 means that no crossovers are performed.
  double crossover_rate() const { return crossover_rate_; }
  void set_crossover_rate(double rate) { crossover_rate_ = rate; }

  // A number between 0.0 and 1.0 specifying the chances of an individual to
  // suffer a mutation between generations. 0.0 means that no mutations are
  // performed.
  double mutation_rate() const { return mutation_rate_; }
  void set_mutation_rate(double rate) { mutation_rate_ = rate; }

  // A number between 0.0 and 1.0 specifying the percentage of the existing
  // population (sorted in descending order based on fitness score) that gets
  // automatically propagated to the next generation population. E.g. If the
  // number is 0.25, it means that at each iteration the best 25% individuals
  // are copied to the next generation.
  //
  // NOTE: The best individual is always propagated to the next generation, no
  //       matter how small this rate is.
  double propagation_rate() const { return propagation_rate_; }
  void set_propagation_rate(double rate) { propagation_rate_ = rate; }

  // The number of generations through which the genetic algorithm will run the
  // population.
  int max_generations() const { return max_generations_; }
  void set_max_generations(int count) { max_generations_ = count; }

  // Simulate the evolution of the initial population across |max_generations_|.
  void Run() {
    delegate_->GetInitialPopulation(population_size_, &population_);
    while (current_generation_ < max_generations_) {
      NextGeneration();
    }
  }

  // Returns the best individual found after the simulation. It must be called
  // only after the Run() method completes.
  Chromosome best() const { return *best_; }

 private:
  // Comparator used to sort the individuals in a population in descending order
  // based on their fitness score.
  class Comparator {
   public:
    explicit Comparator(Delegate* delegate) : delegate_(delegate) {}

    bool operator()(const Chromosome& c1, const Chromosome& c2) {
      return delegate_->Fitness(c1) > delegate_->Fitness(c2);
    }

   private:
    Delegate* const delegate_;
  };

  // This method performs one step of the simulation.
  //   - Sorts the individuals based on their fitness score;
  //   - The best individual is automatically propagated to the next generation;
  //   - Fills in the remaining of the next generation by performing a crossover
  //     between two selected individuals or by simply propagating one
  //     individual to the next generation. The bias is given by the
  //     |crossover_rate_|;
  //   - Performs the mutations on the new population. The canches of an
  //     individual to suffer a mutation are given by the |mutation_rate_|;
  //   - Stores the best individual found so far;
  //   - Calls the user's ReportProgress() callback.
  void NextGeneration() {
    ++current_generation_;
    delegate_->Process(&population_);
    DCHECK_EQ(population_size_, int(population_.size()));
    std::sort(population_.begin(), population_.end(),
        Comparator(Get(delegate_)));
    Population new_population;
    new_population.insert(new_population.end(), population_.begin(),
        population_.begin() + propagation_rate_ * population_size_);
    if (new_population.empty()) {
      new_population.push_back(population_[0]);
    }
    while (new_population.size() < population_.size()) {
      const Chromosome& mate1 = delegate_->Selection(population_);
      if (base::Random() < crossover_rate_) {
        const Chromosome& mate2 = delegate_->Selection(population_);
        delegate_->Crossover(mate1, mate2, &new_population);
      } else {
        new_population.push_back(Chromosome(mate1));
      }
    }
    while (new_population.size() > population_.size()) {
      new_population.pop_back();
    }
    for (int i = 1; i < population_size_; ++i) {
      if (base::Random() < mutation_rate_) {
        delegate_->Mutate(&new_population[i]);
      }
    }
    std::swap(population_, new_population);
    best_ = &population_[0];
    delegate_->ReportProgress(current_generation_,
                              delegate_->Fitness(population_[0]),
                              population_[0]);
  }

  base::ptr::scoped_ptr<Delegate> delegate_;
  Population population_;
  int population_size_;
  double crossover_rate_;
  double mutation_rate_;
  double propagation_rate_;
  int max_generations_;
  int current_generation_;
  Chromosome* best_;

  DISALLOW_COPY_AND_ASSIGN(GeneticAlgorithm);
};

}  // namespace alphabeta
}  // namespace ai

#endif  // AI_ALPHABETA_GENETIC_ALGORITHM_H_

