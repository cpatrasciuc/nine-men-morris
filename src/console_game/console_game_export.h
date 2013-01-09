// Copyright (c) 2013 Cristian Patrasciuc. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef CONSOLE_GAME_CONSOLE_GAME_EXPORT_H_
#define CONSOLE_GAME_CONSOLE_GAME_EXPORT_H_

#ifdef CONSOLE_GAME_IMPLEMENTATION
#define CONSOLE_GAME_EXPORT __attribute__((visibility("default")))
#else
#define CONSOLE_GAME_EXPORT
#endif

#endif  // CONSOLE_GAME_CONSOLE_GAME_EXPORT_H_

