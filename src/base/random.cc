// Copyright (c) 2013 Cristian Patrasciuc. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include <stdint.h>

#include <cstdlib>
#include <vector>

#include "base/random.h"

namespace base {

// TODO(random): Test the implementation using well-known test (e.g. DieHarder)

// All constants are static
const size_t MersenneTwister32::kStateSize                  = 624;
const size_t MersenneTwister32::kShiftSize                  = 397;
const uint32_t MersenneTwister32::kBitmask                  = 0x7fffffffUL;
const uint32_t MersenneTwister32::kXORBitmask               = 0x9908b0dfUL;
const uint32_t MersenneTwister32::kTamperingU               = 11;
const uint32_t MersenneTwister32::kTamperingD               = 0xffffffffUL;
const uint32_t MersenneTwister32::kTamperingS               = 7;
const uint32_t MersenneTwister32::kTamperingB               = 0x9d2c5680UL;
const uint32_t MersenneTwister32::kTamperingT               = 15;
const uint32_t MersenneTwister32::kTamperingC               = 0xefc60000UL;
const uint32_t MersenneTwister32::kTamperingL               = 18;
const uint32_t MersenneTwister32::kInitializationMultiplier = 0x6c078965UL;

MersenneTwister32::MersenneTwister32(uint32_t seed) {
  mt_.resize(kStateSize);
  index_ = 0;
  mt_[index_] = seed;
  for (size_t i = 1; i < mt_.size(); ++i) {
    mt_[i] = kInitializationMultiplier * (mt_[i-1] ^ (mt_[i-1] >> 30)) + i;
  }
}

uint32_t MersenneTwister32::Next() {
  if (index_ == 0) {
    GenerateNumbers();
  }
  uint32_t result = mt_[index_];
  result ^= ((result >> kTamperingU) & kTamperingD);
  result ^= ((result << kTamperingS) & kTamperingB);
  result ^= ((result << kTamperingT) & kTamperingC);
  result ^= (result >> kTamperingL);
  index_ = (index_ + 1) % mt_.size();
  return result;
}

void MersenneTwister32::GenerateNumbers() {
  for (size_t i = 0; i < mt_.size(); ++i) {
    uint32_t x = (mt_[i] & (~kBitmask)) + (mt_[(i+1) % mt_.size()] & kBitmask);
    mt_[i] = mt_[(i + kShiftSize) % mt_.size()] ^ (x >> 1);
    if (x % 2 != 0) {
      mt_[i] ^= kXORBitmask;
    }
  }
}

double Random(double max) {
  return static_cast<double>(std::rand()) / RAND_MAX * max;  // NOLINT
}

}  // namespace base
