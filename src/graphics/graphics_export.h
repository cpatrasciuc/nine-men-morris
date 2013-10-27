// Copyright (c) 2013 Cristian Patrasciuc. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef GRAPHICS_GRAPHICS_EXPORT_H_
#define GRAPHICS_GRAPHICS_EXPORT_H_

#ifdef GRAPHICS_IMPLEMENTATION
#define GRAPHICS_EXPORT __attribute__((visibility("default")))
#else
#define GRAPHICS_EXPORT
#endif

#endif  // GRAPHICS_GRAPHICS_EXPORT_H_
