// Copyright (c) 2012 Cristian Patrasciuc. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef BASE_BASIC_MACROS_H_
#define BASE_BASIC_MACROS_H_

// Use this to get the number of elements for hard-coded arrays.
// Example:
//   int numbers[] = { 10, 20, 30 };
//   for (int i = 0; i < arraysize(numbers); ++i) {
//     ...
//   }
#define arraysize(a) (sizeof(a)/sizeof((a)[0]))

#endif  // BASE_BASIC_MACROS_H_

