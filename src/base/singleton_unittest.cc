// Copyright (c) 2013 Cristian Patrasciuc. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "base/function.h"
#include "base/location.h"
#include "base/singleton.h"
#include "base/singleton_unittest_helper.h"
#include "base/threading/thread_pool_for_unittests.h"
#include "gtest/gtest.h"

namespace base {
namespace {

class Base {
 public:
  int value;
};

// NOTE: In order to reduce verbosity, the *Singleton classes used by this test
// are not declaring their 'tors private/protected. There is no test that
// instantiates directly an instance of a *Singleton type.

class BasicSingleton
    : public Base, public Singleton<BasicSingleton> {
};

class SingleThreadedSingleton
    : public Base, public Singleton<SingleThreadedSingleton, false> {
};

class MultiThreadedSingleton
    : public Base, public Singleton<MultiThreadedSingleton, true> {
};

void GetMultiThreadedSingleton() {
  MultiThreadedSingleton::Instance();
}

template <class T>
class MyCreator {
 public:
  static T* Create() {
    static T instance;
    instance.value = 20;
    return &instance;
  }
};

class CustomCreatorSingleton
    : public Base, public Singleton<CustomCreatorSingleton, false, MyCreator> {
};

template <class T>
void BasicTest() {
  const T* const instance1 = &T::Instance();
  const T* const instance2 = &T::Instance();
  EXPECT_EQ(instance1, instance2);
  T::Instance().value = 10;
  EXPECT_EQ(10, T::Instance().value);
}

TEST(Singleton, Basic) {
  BasicTest<BasicSingleton>();
}

TEST(Singleton, SingleThreaded) {
  BasicTest<SingleThreadedSingleton>();
}

TEST(Singleton, MultiThreaded) {
  base::threading::ThreadPoolForUnittests thread_pool(4);
  thread_pool.CreateThreads();
  for (int i = 0; i < thread_pool.thread_count(); ++i) {
    thread_pool.SubmitTask(i, FROM_HERE,
        new base::Function<void(void)>(&GetMultiThreadedSingleton));
  }
  thread_pool.StartThreads();
  thread_pool.StopAndJoinThreads();
  BasicTest<MultiThreadedSingleton>();
}

TEST(Singleton, DifferentDynamicSharedObjects) {
  const int* const other_dso_instance = GetSingletonFromOtherDSO();
  const int* const this_dso_instance = &Singleton<int>::Instance();
  EXPECT_EQ(this_dso_instance, other_dso_instance);
  Singleton<int>::Instance() = 10;
  EXPECT_EQ(10, *GetSingletonFromOtherDSO());
}

TEST(Singleton, CustomCreator) {
  EXPECT_EQ(20, CustomCreatorSingleton::Instance().value);
  BasicTest<CustomCreatorSingleton>();
}

}  // anonymous namespace
}  // namespace base
