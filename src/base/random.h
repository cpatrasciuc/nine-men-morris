// Copyright (c) 2013 Cristian Patrasciuc. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef BASE_RANDOM_H_
#define BASE_RANDOM_H_

#include <stdint.h>

#include <cstdlib>
#include <vector>

#include "base/base_export.h"
#include "base/basic_macros.h"

namespace base {

// Pseudo random number generator that uses the MT19937 algorithm.
// http://en.wikipedia.org/wiki/Mersenne_twister
class BASE_EXPORT MersenneTwister32 {
 public:
  explicit MersenneTwister32(uint32_t seed = 5489UL);

  // Returns the next unsigned 32-bit random number.
  uint32_t Next();

 private:
  // Update the internal state after each kStateSize-th call to Next().
  void GenerateNumbers();

  // Constants specific to the Mersenne Twister algorithm.
  static const size_t kStateSize;
  static const size_t kShiftSize;
  static const uint32_t kBitmask;
  static const uint32_t kXORBitmask;
  static const uint32_t kTamperingU;
  static const uint32_t kTamperingD;
  static const uint32_t kTamperingS;
  static const uint32_t kTamperingB;
  static const uint32_t kTamperingT;
  static const uint32_t kTamperingC;
  static const uint32_t kTamperingL;
  static const uint32_t kInitializationMultiplier;

  // Stores the internal bits/state of the random number generator.
  std::vector<uint32_t> mt_;

  // Current index used to get the next random number based on the state bits.
  int index_;

  DISALLOW_COPY_AND_ASSIGN(MersenneTwister32);
};

// Utility function used to obtain a random number in the interval [0.0, |max|).
BASE_EXPORT double Random(double max = 1.0);

}  // namespace base

#endif  // BASE_RANDOM_H_

