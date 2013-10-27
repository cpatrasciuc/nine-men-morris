// Copyright (c) 2013 Cristian Patrasciuc. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef BASE_SINGLETON_UNITTEST_HELPER_H_
#define BASE_SINGLETON_UNITTEST_HELPER_H_

#include "base/base_export.h"

namespace base {

// We compile this function in a separate shared library to ensure that we get
// the same singleton instance across multiple shared libraries linked together.
// For more info check the unittest Singleton.DifferentDynamicSharedObjects.
BASE_EXPORT int* GetSingletonFromOtherDSO();

}  // namespace base

#endif  // BASE_SINGLETON_UNITTEST_HELPER_H_

