// Copyright (c) 2013 Cristian Patrasciuc. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "base/singleton_unittest_helper.h"

#include "base/singleton.h"

namespace base {

int* GetSingletonFromOtherDSO() {
  return &Singleton<int>::Instance();
}

}  // namespace base
