// Copyright (c) 2012 Cristian Patrasciuc. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "base/ptr/ref_counted.h"

#include "base/log.h"
#include "base/threading/atomic.h"

namespace base {
namespace ptr {

RefCountedThreadSafe::RefCountedThreadSafe() : ref_count_(0) {}

RefCountedThreadSafe::~RefCountedThreadSafe() {
  DCHECK_EQ(ref_count_.Get(), 0) << "Manually deleted RefCounted object";
}

void RefCountedThreadSafe::AddRef() const {
  ref_count_.Increment();
}

bool RefCountedThreadSafe::Release() const {
  return ref_count_.Decrement() == 0;
}

}  // namespace ptr
}  // namespace base
