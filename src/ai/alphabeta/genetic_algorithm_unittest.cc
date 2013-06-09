// Copyright (c) 2013 Cristian Patrasciuc. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include <bitset>
#include <cstdlib>
#include <memory>

#include "ai/alphabeta/genetic_algorithm.h"
#include "base/basic_macros.h"
#include "base/random.h"
#include "gtest/gtest.h"

namespace ai {
namespace alphabeta {
namespace {

typedef std::bitset<10> TestChromosome;

class TestDelegate : public GeneticAlgorithm<TestChromosome>::Delegate {
 public:
  typedef GeneticAlgorithm<TestChromosome>::Population Population;

  TestDelegate() {}

  virtual void GetInitialPopulation(int size, Population* population) {
    for (int i = 0; i < size; ++i) {
      const int number = base::Random(1e10);
      population->push_back(TestChromosome(number));
    }
  }

  virtual double Fitness(const TestChromosome& individual) {
    return individual.count();
  };

  virtual void Crossover(const TestChromosome& c1,
                         const TestChromosome& c2,
                         Population* population) {
    TestChromosome offspring1(c1);
    TestChromosome offspring2(c2);
    for (int i = 0; i < 5; ++i) {
      offspring1.set(i, c2.test(i));
      offspring2.set(i, c1.test(i));
    }
    population->push_back(offspring1);
    population->push_back(offspring2);
  }

  virtual void Mutate(TestChromosome* individual) {
    const int idx = base::Random(10);
    individual->flip(idx);
  };

 private:
  DISALLOW_COPY_AND_ASSIGN(TestDelegate);
};

TEST(GeneticAlgorithm, AllOnes) {
  std::auto_ptr<GeneticAlgorithm<TestChromosome>::Delegate> delegate;
  delegate.reset(new TestDelegate());
  GeneticAlgorithm<TestChromosome> genetic_algorithm(delegate);
  genetic_algorithm.set_max_generations(1000);
  genetic_algorithm.set_population_size(100);
  genetic_algorithm.set_propagation_rate(0.33);
  genetic_algorithm.Run();
  const TestChromosome best = genetic_algorithm.best();
  EXPECT_TRUE(best.all()) << best.to_ulong();
}

class TestDelegateNoChanges
    : public GeneticAlgorithm<TestChromosome>::Delegate {
 public:
  typedef GeneticAlgorithm<TestChromosome>::Population Population;

  TestDelegateNoChanges() {}

  virtual double Fitness(const TestChromosome& individual) {
    EXPECT_FALSE(individual.any());
    return individual.count();
  };

  virtual const TestChromosome& Selection(const Population& population) {
    EXPECT_TRUE(false) << "Should never be called";
    return population[0];
  };

  virtual void Crossover(const TestChromosome& c1,
                         const TestChromosome& c2,
                         Population* population) {
    FAIL() << "Should never be called";
  }

  virtual void Mutate(TestChromosome* individual) {
    FAIL() << "Should never be called";
  };

 private:
  DISALLOW_COPY_AND_ASSIGN(TestDelegateNoChanges);
};

TEST(GeneticAlgorithm, NoEvolution) {
  std::auto_ptr<GeneticAlgorithm<TestChromosome>::Delegate> delegate;
  delegate.reset(new TestDelegateNoChanges());
  GeneticAlgorithm<TestChromosome> genetic_algorithm(delegate);
  genetic_algorithm.set_max_generations(1000);
  genetic_algorithm.set_population_size(100);
  genetic_algorithm.set_crossover_rate(0.0);
  genetic_algorithm.set_mutation_rate(0.0);
  genetic_algorithm.set_propagation_rate(1.0);
  genetic_algorithm.Run();
  const TestChromosome best = genetic_algorithm.best();
  EXPECT_FALSE(best.any()) << best.to_ulong();
}

}  // anonymous namespace
}  // namespace alphabeta
}  // namespace ai

