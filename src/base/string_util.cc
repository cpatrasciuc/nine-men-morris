// Copyright (c) 2012 Cristian Patrasciuc. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "base/string_util.h"

namespace base {

extern const char kWhiteSpaceChars[] = " \r\n\t";

template <typename T>
void SplitStringTemplate(const std::basic_string<T>& str,
                         std::vector< std::basic_string<T> >* tokens,
                         const std::basic_string<T>& delimiters) {
  size_t start = 0;
  do {
    start = str.find_first_not_of(delimiters, start);
    if (start != std::basic_string<T>::npos) {
      const size_t end = str.find_first_of(delimiters, start);
      if (end != std::basic_string<T>::npos) {
        tokens->push_back(str.substr(start, end - start));
      } else {
        tokens->push_back(str.substr(start));
      }
      start = end;
    }
  } while (start != std::basic_string<T>::npos);
}

void SplitString(const std::string& str,
                 std::vector<std::string>* tokens,
                 const std::string& delimiters) {
  SplitStringTemplate(str, tokens, delimiters);
}

}  // namespace base
