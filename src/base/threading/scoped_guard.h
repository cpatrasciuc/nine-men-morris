// Copyright (c) 2012 Cristian Patrasciuc. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef BASE_THREADING_SCOPED_GUARD_H_
#define BASE_THREADING_SCOPED_GUARD_H_

#include "base/base_export.h"
#include "base/basic_macros.h"

namespace base {
namespace threading {

class Lock;

class BASE_EXPORT ScopedGuard {
 public:
  explicit ScopedGuard(Lock* lock);

  ~ScopedGuard();

 private:
  Lock* lock_;

  DISALLOW_COPY_AND_ASSIGN(ScopedGuard);
};

}  // namespace threading
}  // namespace base

#endif  // BASE_THREADING_SCOPED_GUARD_H_

