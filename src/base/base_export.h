// Copyright (c) 2012 Cristian Patrasciuc. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef BASE_BASE_EXPORT_H_
#define BASE_BASE_EXPORT_H_

#ifdef BASE_IMPLEMENTATION
#define BASE_EXPORT __attribute__((visibility("default")))
#else
#define BASE_EXPORT
#endif

#endif  // BASE_BASE_EXPORT_H_

