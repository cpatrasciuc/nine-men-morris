// Copyright (c) 2013 Cristian Patrasciuc. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// This implementation is inspired from the "Implementing Singletons" chapter
// in "Modern C++ Design: Generic Programming and Design Patterns Applied" by
// Andrei Alexandrescu.

#ifndef BASE_SINGLETON_H_
#define BASE_SINGLETON_H_

#include "base/basic_macros.h"
#include "base/threading/lock.h"

namespace base {

// Default creation policy for a singleton. Requires a no-argument constructor
// for the singleton type. Also, uses C++ rules to automatically delete the
// single instance before the application exits.
template <class T>
class DefaultCreationPolicy {
 public:
  static T* Create() {
    static T instance;
    return &instance;
  }
};

template <class T, bool multi_threaded> class ThreadingModel;

// Threading model for singletons that are only used on one thread. There is no
// need to make any "volatile" declaration nor to acquire any lock.
template <class T>
class ThreadingModel<T, false> {
 public:
  typedef T VolatileType;

  class Lock {
   public:
    Lock() {}
   private:
    DISALLOW_COPY_AND_ASSIGN(Lock);
  };
};

// Threading model for singletons that are used on multiple threads. It declares
// the pointer to the single instance as volatile and uses a shared lock to
// ensure that initialization is performed only once.
template <class T>
class ThreadingModel<T, true> {
 public:
  typedef volatile T VolatileType;

  class Lock {
   public:
    Lock() : guard_(&g_lock) {}

   private:
    static base::threading::Lock g_lock;
    base::threading::ScopedGuard guard_;

    DISALLOW_COPY_AND_ASSIGN(Lock);
  };
};

// static
template <class T> base::threading::Lock ThreadingModel<T, true>::Lock::g_lock;

// Template class that should be used by classes that need to use the singleton
// design pattern. If a class C must be a singleton it must make its 'tors
// protected and inherit from Singleton<C>.
// The |multi_threaded| template argument must be set to |true| if the singleton
// is accessed from multiple threads.
// The |CreationPolicy| must provide a static method called Create() that is
// responsible for initializing the singleton instance.
template <class T,
          bool multi_threaded = true,
          template <class> class CreationPolicy = DefaultCreationPolicy>
class Singleton {
 public:
  static T& Instance() {
    if (!instance_) {
      typename ThreadingModel<T, multi_threaded>::Lock _;
      if (!instance_) {
        instance_ = CreationPolicy<T>::Create();
      }
    }
    return const_cast<T&>(*instance_);
  }

 protected:
  Singleton() {}
  virtual ~Singleton() = 0;

 private:
  typedef typename ThreadingModel<T, multi_threaded>::VolatileType InstanceType;
  ALWAYS_EXPORT static InstanceType* instance_;

  DISALLOW_COPY_AND_ASSIGN(Singleton);
};

template <class T, bool multi_threaded, template <class> class CreationPolicy>
Singleton<T, multi_threaded, CreationPolicy>::~Singleton() {}

// static
template <class T, bool multi_threaded, template <class> class CreationPolicy>
typename ThreadingModel<T, multi_threaded>::VolatileType*
Singleton<T, multi_threaded, CreationPolicy>::instance_ = NULL;

}  // namespace base

#endif  // BASE_SINGLETON_H_

