// Copyright (c) 2012 Cristian Patrasciuc. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "base/threading/scoped_guard.h"

#include "base/threading/lock.h"

namespace base {
namespace threading {

ScopedGuard::ScopedGuard(Lock* lock) : lock_(lock) {
  lock_->Acquire();
}

ScopedGuard::~ScopedGuard() {
  lock_->Release();
}

}  // namespace threading
}  // namespace base
