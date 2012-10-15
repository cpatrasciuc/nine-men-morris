// Copyright (c) 2012 Cristian Patrasciuc. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "base/threading/atomic.h"
#include "gtest/gtest.h"

namespace base {
namespace threading {
namespace {

// TODO(threading): Test all possible atomic types

TEST(Atomic, DefaultInitialization) {
  Atomic<int> atomic;
  EXPECT_EQ(0, atomic.Get());
}

TEST(Atomic, Copy) {
  Atomic<int> atomic1(10);
  Atomic<int> atomic2(atomic1);
  EXPECT_EQ(atomic1.Get(), atomic2.Get());
}

TEST(Atomic, BasicOperations) {
  Atomic<int> atomic(2);
  atomic.Add(5);
  EXPECT_EQ(7, atomic.Get());
  atomic.Subtract(3);
  EXPECT_EQ(4, atomic.Get());
  atomic.Increment();
  EXPECT_EQ(5, atomic.Get());
  atomic.Decrement();
  EXPECT_EQ(4, atomic.Get());
  atomic.BitwiseAnd(4);
  EXPECT_EQ(4, atomic.Get());
  atomic.BitwiseAnd(1);
  EXPECT_EQ(0, atomic.Get());
  atomic.BitwiseOr(0);
  EXPECT_EQ(0, atomic.Get());
  atomic.BitwiseOr(10);
  EXPECT_EQ(10, atomic.Get());
  atomic.BitwiseXor(12);
  EXPECT_EQ(6, atomic.Get());
  // TODO(clang): Enable this when clang version is updated
  /*atomic.BitwiseNand(1);
  EXPECT_EQ(1, atomic.Get());*/
  atomic.CompareAndSwap(10, 10);
  EXPECT_EQ(6, atomic.Get());
  atomic.CompareAndSwap(6, 10);
  EXPECT_EQ(10, atomic.Get());
}

}  // anonymous namespace
}  // namespace threading
}  // namespace base
