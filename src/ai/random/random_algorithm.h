// Copyright (c) 2013 Cristian Patrasciuc. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef AI_RANDOM_RANDOM_ALGORITHM_H_
#define AI_RANDOM_RANDOM_ALGORITHM_H_

#include <memory>

#include "ai/ai_algorithm.h"
#include "ai/ai_export.h"
#include "base/callable.h"
#include "base/ptr/scoped_ptr.h"
#include "game/player_action.h"

namespace game {
class Game;
}

namespace ai {
namespace random {

// This class is the most simple AI player. In each situation it randomly
// chooses one of the valid actions that can be made.
// The randomness is given by an external random number generator that is
// provided when a new |RandomAlgorithm| is instantiated. The generator is an
// object that can be called without any arguments and returns a random int.
class AI_EXPORT RandomAlgorithm : public AIAlgorithm {
 public:
  typedef base::Callable<int(void)> RandomNumberGenerator;

  // The no-argument constructor uses std::rand() to generate random numbers. It
  // also calls std::srand(std::time(NULL)).
  RandomAlgorithm();

  // This constructor allows the users of this class to provide their own random
  // number generator.
  explicit RandomAlgorithm(std::auto_ptr<RandomNumberGenerator> random);

 private:
  // AIAlgorithm interface
  virtual game::PlayerAction GetNextAction(const game::Game& game_model);

  base::ptr::scoped_ptr<RandomNumberGenerator> random_number_generator_;
};

}  // namespace random
}  // namespace ai

#endif  // AI_RANDOM_RANDOM_ALGORITHM_H_

