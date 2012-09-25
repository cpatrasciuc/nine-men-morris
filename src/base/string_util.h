// Copyright (c) 2012 Cristian Patrasciuc. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef BASE_STRING_UTIL_H_
#define BASE_STRING_UTIL_H_

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

}  // namespace base

#endif  // BASE_STRING_UTIL_H_

