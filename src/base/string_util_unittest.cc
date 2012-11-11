// Copyright (c) 2012 Cristian Patrasciuc. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include <string>
#include <vector>

#include "base/string_util.h"
#include "gtest/gtest.h"

namespace base {
namespace {

TEST(StringUtilTest, SplitString) {
  std::string s("baaabccbbbdbb");
  std::vector<std::string> v;
  std::string delim("b");
  SplitString(s, &v, delim);
  ASSERT_EQ(3U, v.size());
  EXPECT_EQ("aaa", v[0]);
  EXPECT_EQ("cc", v[1]);
  EXPECT_EQ("d", v[2]);

  v.clear();
  s = "bbbb";
  SplitString(s, &v, delim);
  EXPECT_EQ(0U, v.size());

  v.clear();
  s = "aaa";
  SplitString(s, &v, delim);
  ASSERT_EQ(1U, v.size());
  EXPECT_EQ(s, v[0]);

  v.clear();
  SplitString(std::string(), &v, delim);
  EXPECT_EQ(0U, v.size());

  v.clear();
  SplitString("aaabbbbbbccbbd", &v, delim);
  ASSERT_EQ(3U, v.size());
  EXPECT_EQ("aaa", v[0]);
  EXPECT_EQ("cc", v[1]);
  EXPECT_EQ("d", v[2]);

  v.clear();
  SplitString(" \taaa   cc\n  d\r\n", &v);
  ASSERT_EQ(3U, v.size());
  EXPECT_EQ("aaa", v[0]);
  EXPECT_EQ("cc", v[1]);
  EXPECT_EQ("d", v[2]);
}

TEST(StringUtilTest, ToString) {
  EXPECT_EQ("10", ToString(10));
  EXPECT_EQ("-123.45", ToString(-123.450));
  EXPECT_EQ("0", ToString(static_cast<void*>(NULL)));
  EXPECT_EQ("test", ToString("test"));
  char c = 10;
  EXPECT_EQ("\n", ToString(c));
  EXPECT_EQ("10", ToString<int>(c));
}

}  // anonymous namespace
}  // namespace base
