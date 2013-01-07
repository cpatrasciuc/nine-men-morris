// Copyright (c) 2012 Cristian Patrasciuc. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef BASE_STRING_UTIL_H_
#define BASE_STRING_UTIL_H_

#include <sstream>
#include <string>
#include <vector>

#include "base/base_export.h"

namespace base {

BASE_EXPORT extern const char kWhiteSpaceChars[];

// Splits the |str| string into tokens, using the characters from |delimiters|
// as separators. Each token is appended to the |tokens| vector.
// The |tokens| vector is NOT cleared by this function before appending to it.
BASE_EXPORT void SplitString(const std::string& str,
                             std::vector<std::string>* tokens,
                             const std::string& delimiters = kWhiteSpaceChars);

// Utility function that converts an object of type T to a string. The type T
// must support operator <<.
//
// WARNING: Be careful when using it for 'char' or 'int8_t' since it will treat
// the argument as a character not as a number.
//
//   int8_t x = 10;
//   EXPECT_EQ("\n", ToString(x));       /* Treats it as a char */
//   EXPECT_EQ("10", ToString<int>(x));  /* Force it to treat it a number */
//
template <typename T>
std::string ToString(T t) {
  std::ostringstream oss;
  oss << t;
  return oss.str();
}

// Compares two strings ignoring the case. It does not perform any copies.
// The meaning of the return value is the following:
//   - return value is 0 if a == b
//   - return value is negative if a < b
//   - return value is positive if a > b.
BASE_EXPORT int CompareIgnoreCase(const std::string& a, const std::string& b);

// Transform a string to lowercase.
BASE_EXPORT std::string ToLowerCase(const std::string& str);

// Removes whitespace at the beginning and at the end of the string.
BASE_EXPORT std::string StripString(const std::string& str);

}  // namespace base

#endif  // BASE_STRING_UTIL_H_

