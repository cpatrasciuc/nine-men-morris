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

// TODO(macros): Add a macro from __attribute__(override)

// This macro should be used in tests for specifying that specific statements
// should not compile. When the unit under test is modified it's easy to quickly
// check that the compile-time restrictions are still valid by temporarily 
// removing the macros. It also helps to understand better the expected
// behavior of the class that is tested.
#define SHOULD_NOT_COMPILE(statement)

#endif  // BASE_BASIC_MACROS_H_

