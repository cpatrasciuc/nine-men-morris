// Copyright (c) 2013 Cristian Patrasciuc. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "base/random.h"
#include "gtest/gtest.h"

namespace base {
namespace {

TEST(Random, MersenneTwister32) {
  const uint32_t expected_values[] = {
      // Expected for the default seed
      3499211612,
      1185518681,
      696567687,
      1792148406,
      2767325310,
      4136433784,
      256061060,
      1294739153,
      1816280216,
      3934152427,
      2500741117,
      // Expected for seed = 12345
      3992670690,
      2282559898,
      840933102,
      3790886053,
      3637592465,
      447125268,
      1271688334,
      926638379,
      3946885367,
      1683921108,
      984001283
  };
  size_t index = 0;
  MersenneTwister32 mt32_default_seed;
  for (int i = 0; i < 1001; ++i) {
    const uint32_t x = mt32_default_seed.Next();
    if (i % 100 == 0) {
      EXPECT_EQ(expected_values[index++], x) << i;
    }
  }
  MersenneTwister32 mt32(12345U);
  for (int i = 0; i < 1001; ++i) {
    const uint32_t x = mt32.Next();
    if (i % 100 == 0) {
      EXPECT_EQ(expected_values[index++], x) << i;
    }
  }
}

TEST(Random, TwoGeneratorsWithSameSeed) {
  const uint32_t seed = 987654321;
  MersenneTwister32 first(seed);
  MersenneTwister32 second(seed);
  for (int i = 0; i < 1000; ++i) {
    EXPECT_EQ(first.Next(), second.Next()) << i;
  }
}

}  // anonymous namespace
}  // namespace base
