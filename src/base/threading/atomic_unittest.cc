// Copyright (c) 2012 Cristian Patrasciuc. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include <stdint.h>

#include "base/threading/atomic.h"
#include "gtest/gtest.h"

namespace base {
namespace threading {
namespace {

template <typename T>
class AtomicTest : public ::testing::Test {};

typedef ::testing::Types<int8_t,  uint8_t,
                         int16_t, uint16_t,
                         int32_t, uint32_t,
                         int64_t, uint64_t> AllowedAtomicTypes;
TYPED_TEST_CASE(AtomicTest, AllowedAtomicTypes);

TYPED_TEST(AtomicTest, DefaultInitialization) {
  Atomic<TypeParam> atomic;
  EXPECT_EQ(TypeParam(0), atomic.Get());
}

TYPED_TEST(AtomicTest, Copy) {
  Atomic<TypeParam> atomic1(10);
  Atomic<TypeParam> atomic2(atomic1);
  EXPECT_EQ(atomic1.Get(), atomic2.Get());
}

TYPED_TEST(AtomicTest, BasicOperations) {
  Atomic<TypeParam> atomic(2);
  EXPECT_EQ(TypeParam(2), atomic.Get());
  atomic.Add(5);
  EXPECT_EQ(TypeParam(7), atomic.Get());
  atomic.Subtract(3);
  EXPECT_EQ(TypeParam(4), atomic.Get());
  atomic.Increment();
  EXPECT_EQ(TypeParam(5), atomic.Get());
  atomic.Decrement();
  EXPECT_EQ(TypeParam(4), atomic.Get());
  atomic.BitwiseAnd(4);
  EXPECT_EQ(TypeParam(4), atomic.Get());
  atomic.BitwiseAnd(1);
  EXPECT_EQ(TypeParam(0), atomic.Get());
  atomic.BitwiseOr(0);
  EXPECT_EQ(TypeParam(0), atomic.Get());
  atomic.BitwiseOr(10);
  EXPECT_EQ(TypeParam(10), atomic.Get());
  atomic.BitwiseXor(12);
  EXPECT_EQ(TypeParam(6), atomic.Get());
  // TODO(clang): Enable this when clang supports atomic nand
  // atomic.BitwiseNand(1);
  // EXPECT_EQ(TypeParam(1), atomic.Get());
  atomic.CompareAndSwap(10, 10);
  EXPECT_EQ(TypeParam(6), atomic.Get());
  atomic.CompareAndSwap(6, 10);
  EXPECT_EQ(TypeParam(10), atomic.Get());
}

}  // anonymous namespace
}  // namespace threading
}  // namespace base
