// Copyright (c) 2013 Cristian Patrasciuc. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef AI_AI_EXPORT_H_
#define AI_AI_EXPORT_H_

#ifdef AI_IMPLEMENTATION
#define AI_EXPORT __attribute__((visibility("default")))
#else
#define AI_EXPORT
#endif

#endif  // AI_AI_EXPORT_H_
