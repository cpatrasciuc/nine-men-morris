// Copyright (c) 2013 Cristian Patrasciuc. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include <bitset>
#include <cstdlib>
#include <memory>

#include "ai/alphabeta/genetic_algorithm.h"
#include "base/basic_macros.h"
#include "gtest/gtest.h"

namespace ai {
namespace alphabeta {
namespace {

typedef std::bitset<10> TestChromosome;

class TestDelegate : public GeneticAlgorithm<TestChromosome>::Delegate {
 public:
  typedef std::vector<TestChromosome> Population;

  TestDelegate() {}

  virtual void GetInitialPopulation(int size, Population* population) {
    for (int i = 0; i < size; ++i) {
      const int number =
          static_cast<double>(std::rand()) / RAND_MAX * 1e10;  // NOLINT
      population->push_back(TestChromosome(number));
    }
  }

  virtual void Tournament(const Population& population) {}

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
    const int idx = static_cast<double>(std::rand()) / RAND_MAX * 10;  // NOLINT
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
  genetic_algorithm.Run();
  const TestChromosome best = genetic_algorithm.best();
  EXPECT_TRUE(best.all()) << best.to_ulong();
}

}  // anonymous namespace
}  // namespace alphabeta
}  // namespace ai

