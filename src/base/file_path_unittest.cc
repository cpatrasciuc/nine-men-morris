// Copyright (c) 2012 Cristian Patrasciuc. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "base/basic_macros.h"
#include "base/file_path.h"
#include "gtest/gtest.h"

namespace base {
namespace {

// TODO(file_path): Change the hard coded paths to use FILE_PATH_LITERAL

TEST(FilePath, Basic) {
  const FilePath empty_path;
  EXPECT_TRUE(empty_path.empty());
  EXPECT_EQ("", empty_path.value());
  const FilePath non_empty_path("/foo/bar.baz");
  EXPECT_FALSE(non_empty_path.empty());
  EXPECT_EQ("/foo/bar.baz", non_empty_path.value());
}

TEST(FilePath, StaticUtilities) {
  EXPECT_EQ(".", FilePath::CurrentDir().value());
  EXPECT_EQ("..", FilePath::ParentDir().value());
}

TEST(FilePath, DirName) {
  const char* const tests[] = {
    "", ".",
    "/foo/bar/baz.txt", "/foo/bar",
    "bar/baz.txt", "bar",
    "baz.txt", ".",
    "/foo/bar/baz", "/foo/bar",
    "/foo/bar/baz/", "/foo/bar",
    "/foo/bar/baz//", "/foo/bar",
    "foo/", ".",
    "/foo", "/",
    "/foo/", "/",
    "foo/bar", "foo",
    "foo/bar/", "foo",
    "foo/bar//", "foo",
    "foo//bar//", "foo",
    ".", ".",
    "/", "/",
    "//", "//",
    "///", "/"
  };
  for (size_t i = 0; i < arraysize(tests); i += 2) {
    const FilePath file_path(tests[i]);
    EXPECT_EQ(tests[i + 1], file_path.DirName().value()) << i;
  }
}

TEST(FilePath, BaseName) {
  const char* const tests[] = {
    "", ".",
    "foo", "foo",
    "/fo/bar", "bar",
    "/foo/bar/", "bar",
    "/foo/bar//", "bar",
    "/foo/bar/baz.txt", "baz.txt",
    "/foo", "foo",
    "/", "/",
    "//", "/",
    "///", "/",
    "foo/", "foo",
    "foo/bar", "bar",
    "foo/bar/", "bar",
    "foo/bar//", "bar",
    "foo//bar//", "bar",
    "foo//bar/", "bar",
    "foo//bar", "bar",
    "//foo/bar", "bar",
    "//foo/", "foo",
    "//foo", "foo"
  };
  for (size_t i = 0; i < arraysize(tests); i += 2) {
    const FilePath file_path(tests[i]);
    EXPECT_EQ(tests[i + 1], file_path.BaseName().value()) << i/2;
  }
}

TEST(FilePath, IsAbsolute) {
  EXPECT_TRUE(FilePath("/").IsAbsolute());
  EXPECT_TRUE(FilePath("//").IsAbsolute());
  EXPECT_TRUE(FilePath("///").IsAbsolute());
  EXPECT_TRUE(FilePath("/foo").IsAbsolute());
  EXPECT_FALSE(FilePath("").IsAbsolute());
  EXPECT_FALSE(FilePath(".").IsAbsolute());
  EXPECT_FALSE(FilePath("..").IsAbsolute());
  EXPECT_FALSE(FilePath("foo/").IsAbsolute());
  EXPECT_FALSE(FilePath("foo/bar").IsAbsolute());
}

TEST(FilePath, Extension) {
  EXPECT_EQ("", FilePath("").Extension().value());
  EXPECT_EQ("", FilePath("foo").Extension().value());
  EXPECT_EQ("", FilePath("/foo").Extension().value());
  EXPECT_EQ("", FilePath("/foo/").Extension().value());
  EXPECT_EQ("", FilePath("foo.txt/").Extension().value());
  EXPECT_EQ(".txt", FilePath("foo.txt").Extension().value());
  EXPECT_EQ(".txt", FilePath("/foo/bar/baz.txt").Extension().value());
}

TEST(FilePath, AddExtension) {
  const char* const tests[] = {
    "", "", "",
    "", "txt", "",
    ".", "txt", "",
    "..", "txt", "",
    ".", "", "",
    "foo.dll", "txt", "foo.dll.txt",
    "./foo.dll", "txt", "./foo.dll.txt",
    "foo..dll", "txt", "foo..dll.txt",
    "foo.dll", ".txt", "foo.dll.txt",
    "foo", "txt", "foo.txt",
    "foo.", "txt", "foo..txt",
    "foo..", "txt", "foo...txt",
    "foo", ".txt", "foo.txt",
    "foo.baz.dll", "txt", "foo.baz.dll.txt",
    "foo.baz.dll", ".txt", "foo.baz.dll.txt",
    "foo.dll", "", "foo.dll",
    "foo.dll", ".", "foo.dll",
    "foo", "", "foo",
    "foo", ".", "foo",
    "foo.baz.dll", "", "foo.baz.dll",
    "foo.baz.dll", ".", "foo.baz.dll",
  };
  for (size_t i = 0; i < arraysize(tests); i += 3) {
    const FilePath file_path(tests[i]);
    const FilePath extension(tests[i + 1]);
    EXPECT_EQ(tests[i + 2], file_path.AddExtension(extension).value()) << i/3;
  }
}

TEST(FilePath, StripTrailingSeparators) {
  const char* const tests[] = {
    "", "",
    "/", "/",
    "//", "/",
    "///", "/",
    "////", "/",
    "foo/", "foo",
    "foo//", "foo",
    "foo///", "foo",
    "foo////", "foo",
    "/foo", "/foo",
    "/foo/", "/foo",
    "/foo//", "/foo",
    "/foo///", "/foo",
    "/foo////", "/foo",
  };
  for (size_t i = 0; i < arraysize(tests); i += 2) {
    const FilePath file_path(tests[i]);
    EXPECT_EQ(tests[i + 1], file_path.StripTrailingSeparators().value()) << i/2;
  }
}

TEST(FilePath, Append) {
  const char* const tests[] = {
    "", "foo", "foo",
    ".", "bar", "./bar",
    "/", "foo", "/foo",
    "/foo", "", "/foo",
    "/foo/", "", "/foo",
    "//foo", "", "//foo",
    "//foo/", "", "//foo",
    "//", "foo", "/foo"
  };
  for (size_t i = 0; i < arraysize(tests); i += 3) {
    const FilePath initial_path(tests[i]);
    const FilePath path_to_append(tests[i + 1]);
    EXPECT_EQ(tests[i + 2], initial_path.Append(path_to_append).value()) << i/3;
    EXPECT_EQ(tests[i + 2], initial_path.Append(tests[i + 1]).value()) << i/3;
  }
}

TEST(FilePath, Exists) {
  const FilePath current_file(__FILE__);
  EXPECT_TRUE(current_file.Exists());
  EXPECT_TRUE(current_file.DirName().Exists());
  EXPECT_FALSE(current_file.Append("0123456789").Exists());
}

}  // anonymous namespace
}  // namespace base
