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

// Force the visibility of a symbol to be public.
#define ALWAYS_EXPORT __attribute__((visibility("default")))

// TODO(macros): Add a macro from __attribute__(override)

// This macro should be used in tests for specifying that specific statements
// should not compile. When the unit under test is modified it's easy to quickly
// check that the compile-time restrictions are still valid by temporarily
// removing the macros. It also helps to understand better the expected
// behavior of the class that is tested.
#define SHOULD_NOT_COMPILE(statement)

// A macro to disallow the copy constructor and operator= functions
// This should be used in the private: declarations for a class
#define DISALLOW_COPY_AND_ASSIGN(TypeName) \
  TypeName(const TypeName&);               \
  void operator=(const TypeName&);

// This macro should be used for test cases that must only be run on debug mode
// builds. Usually these represent death tests that break different DCHECK's.
// Usage:
//   TEST(SomeClassDeathTest, DEBUG_ONLY_TEST(InvalidArgumentsTest)).
#if defined(DEBUG_MODE)
#define DEBUG_ONLY_TEST(testcase) testcase
#else
#define DEBUG_ONLY_TEST(testcase) DISABLED_##testcase
#endif

#endif  // BASE_BASIC_MACROS_H_

