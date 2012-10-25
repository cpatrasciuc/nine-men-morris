// Copyright (c) 2012 Cristian Patrasciuc. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef GAME_GAME_EXPORT_H_
#define GAME_GAME_EXPORT_H_

#ifdef GAME_IMPLEMENTATION
#define GAME_EXPORT __attribute__((visibility("default")))
#else
#define GAME_EXPORT
#endif

#endif  // GAME_GAME_EXPORT_H_


