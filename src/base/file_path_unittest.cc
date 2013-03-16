// Copyright (c) 2012 Cristian Patrasciuc. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include <sys/stat.h>
#include <unistd.h>

#include <fstream>
#include <string>
#include <vector>

#include "base/basic_macros.h"
#include "base/file_path.h"
#include "base/file_util.h"
#include "base/log.h"
#include "gtest/gtest.h"

namespace base {
namespace {

#define FPL(x) FILE_PATH_LITERAL(x)

TEST(FilePath, Basic) {
  const FilePath empty_path;
  EXPECT_TRUE(empty_path.empty());
  EXPECT_EQ(FPL(""), empty_path.value());
  const FilePath non_empty_path(FPL("/foo/bar.baz"));
  EXPECT_FALSE(non_empty_path.empty());
  EXPECT_EQ(FPL("/foo/bar.baz"), non_empty_path.value());
}

TEST(FilePath, StaticUtilities) {
  EXPECT_EQ(FPL("."), FilePath::CurrentDir().value());
  EXPECT_EQ(FPL(".."), FilePath::ParentDir().value());
}

TEST(FilePath, DirName) {
  const char* const tests[] = {
    FPL(""), FPL("."),
    FPL("/foo/bar/baz.txt"), FPL("/foo/bar"),
    FPL("bar/baz.txt"), FPL("bar"),
    FPL("baz.txt"), FPL("."),
    FPL("/foo/bar/baz"), FPL("/foo/bar"),
    FPL("/foo/bar/baz/"), FPL("/foo/bar"),
    FPL("/foo/bar/baz//"), FPL("/foo/bar"),
    FPL("foo/"), FPL("."),
    FPL("/foo"), FPL("/"),
    FPL("/foo/"), FPL("/"),
    FPL("foo/bar"), FPL("foo"),
    FPL("foo/bar/"), FPL("foo"),
    FPL("foo/bar//"), FPL("foo"),
    FPL("foo//bar//"), FPL("foo"),
    FPL("."), FPL("."),
    FPL("/"), FPL("/"),
    FPL("//"), FPL("//"),
    FPL("///"), FPL("/")
  };
  for (size_t i = 0; i < arraysize(tests); i += 2) {
    const FilePath file_path(tests[i]);
    EXPECT_EQ(tests[i + 1], file_path.DirName().value()) << i;
  }
}

TEST(FilePath, BaseName) {
  const char* const tests[] = {
    FPL(""), FPL("."),
    FPL("foo"), FPL("foo"),
    FPL("/fo/bar"), FPL("bar"),
    FPL("/foo/bar/"), FPL("bar"),
    FPL("/foo/bar//"), FPL("bar"),
    FPL("/foo/bar/baz.txt"), FPL("baz.txt"),
    FPL("/foo"), FPL("foo"),
    FPL("/"), FPL("/"),
    FPL("//"), FPL("/"),
    FPL("///"), FPL("/"),
    FPL("foo/"), FPL("foo"),
    FPL("foo/bar"), FPL("bar"),
    FPL("foo/bar/"), FPL("bar"),
    FPL("foo/bar//"), FPL("bar"),
    FPL("foo//bar//"), FPL("bar"),
    FPL("foo//bar/"), FPL("bar"),
    FPL("foo//bar"), FPL("bar"),
    FPL("//foo/bar"), FPL("bar"),
    FPL("//foo/"), FPL("foo"),
    FPL("//foo"), FPL("foo")
  };
  for (size_t i = 0; i < arraysize(tests); i += 2) {
    const FilePath file_path(tests[i]);
    EXPECT_EQ(tests[i + 1], file_path.BaseName().value()) << i/2;
  }
}

TEST(FilePath, IsAbsolute) {
  EXPECT_TRUE(FilePath(FPL("/")).IsAbsolute());
  EXPECT_TRUE(FilePath(FPL("//")).IsAbsolute());
  EXPECT_TRUE(FilePath(FPL("///")).IsAbsolute());
  EXPECT_TRUE(FilePath(FPL("/foo")).IsAbsolute());
  EXPECT_FALSE(FilePath(FPL("")).IsAbsolute());
  EXPECT_FALSE(FilePath(FPL(".")).IsAbsolute());
  EXPECT_FALSE(FilePath(FPL("..")).IsAbsolute());
  EXPECT_FALSE(FilePath(FPL("foo/")).IsAbsolute());
  EXPECT_FALSE(FilePath(FPL("foo/bar")).IsAbsolute());
}

TEST(FilePath, Extension) {
  EXPECT_EQ(FPL(""), FilePath(FPL("")).Extension().value());
  EXPECT_EQ(FPL(""), FilePath(FPL("foo")).Extension().value());
  EXPECT_EQ(FPL(""), FilePath(FPL("/foo")).Extension().value());
  EXPECT_EQ(FPL(""), FilePath(FPL("/foo/")).Extension().value());
  EXPECT_EQ(FPL(""), FilePath(FPL("foo.txt/")).Extension().value());
  EXPECT_EQ(FPL(".txt"), FilePath(FPL("foo.txt")).Extension().value());
  EXPECT_EQ(FPL(".txt"), FilePath(FPL("/foo/bar/baz.txt")).Extension().value());
  EXPECT_EQ(FPL(".txt"),
            FilePath(FPL("/foo/bar.baz/qux.txt")).Extension().value());
}

TEST(FilePath, AddExtension) {
  const char* const tests[] = {
    FPL(""), FPL(""), FPL(""),
    FPL(""), FPL("txt"), FPL(""),
    FPL("."), FPL("txt"), FPL(""),
    FPL(".."), FPL("txt"), FPL(""),
    FPL("."), FPL(""), FPL(""),
    FPL("foo.dll"), FPL("txt"), FPL("foo.dll.txt"),
    FPL("./foo.dll"), FPL("txt"), FPL("./foo.dll.txt"),
    FPL("foo..dll"), FPL("txt"), FPL("foo..dll.txt"),
    FPL("foo.dll"), FPL(".txt"), FPL("foo.dll.txt"),
    FPL("foo"), FPL("txt"), FPL("foo.txt"),
    FPL("foo."), FPL("txt"), FPL("foo..txt"),
    FPL("foo.."), FPL("txt"), FPL("foo...txt"),
    FPL("foo"), FPL(".txt"), FPL("foo.txt"),
    FPL("foo.baz.dll"), FPL("txt"), FPL("foo.baz.dll.txt"),
    FPL("foo.baz.dll"), FPL(".txt"), FPL("foo.baz.dll.txt"),
    FPL("foo.dll"), FPL(""), FPL("foo.dll"),
    FPL("foo.dll"), FPL("."), FPL("foo.dll"),
    FPL("foo"), FPL(""), FPL("foo"),
    FPL("foo"), FPL("."), FPL("foo"),
    FPL("foo.baz.dll"), FPL(""), FPL("foo.baz.dll"),
    FPL("foo.baz.dll"), FPL("."), FPL("foo.baz.dll")
  };
  for (size_t i = 0; i < arraysize(tests); i += 3) {
    const FilePath file_path(tests[i]);
    const FilePath extension(tests[i + 1]);
    EXPECT_EQ(tests[i + 2], file_path.AddExtension(extension).value()) << i/3;
  }
}

TEST(FilePath, StripTrailingSeparators) {
  const char* const tests[] = {
    FPL(""), FPL(""),
    FPL("/"), FPL("/"),
    FPL("//"), FPL("/"),
    FPL("///"), FPL("/"),
    FPL("////"), FPL("/"),
    FPL("foo/"), FPL("foo"),
    FPL("foo//"), FPL("foo"),
    FPL("foo///"), FPL("foo"),
    FPL("foo////"), FPL("foo"),
    FPL("/foo"), FPL("/foo"),
    FPL("/foo/"), FPL("/foo"),
    FPL("/foo//"), FPL("/foo"),
    FPL("/foo///"), FPL("/foo"),
    FPL("/foo////"), FPL("/foo")
  };
  for (size_t i = 0; i < arraysize(tests); i += 2) {
    const FilePath file_path(tests[i]);
    EXPECT_EQ(tests[i + 1], file_path.StripTrailingSeparators().value()) << i/2;
  }
}

TEST(FilePath, Append) {
  const char* const tests[] = {
    FPL(""), FPL("foo"), FPL("foo"),
    FPL("."), FPL("bar"), FPL("./bar"),
    FPL("/"), FPL("foo"), FPL("/foo"),
    FPL("/foo"), FPL(""), FPL("/foo"),
    FPL("/foo/"), FPL(""), FPL("/foo"),
    FPL("//foo"), FPL(""), FPL("//foo"),
    FPL("//foo/"), FPL(""), FPL("//foo"),
    FPL("//"), FPL("foo"), FPL("/foo")
  };
  for (size_t i = 0; i < arraysize(tests); i += 3) {
    const FilePath initial_path(tests[i]);
    const FilePath path_to_append(tests[i + 1]);
    EXPECT_EQ(tests[i + 2], initial_path.Append(path_to_append).value()) << i/3;
    EXPECT_EQ(tests[i + 2], initial_path.Append(tests[i + 1]).value()) << i/3;
  }
}

TEST(FilePath, Exists) {
  const FilePath current_file(FPL(__FILE__));
  EXPECT_TRUE(current_file.Exists());
  EXPECT_TRUE(current_file.DirName().Exists());
  EXPECT_FALSE(current_file.Append(FPL("0123456789")).Exists());
}

TEST(FilePath, IsDir) {
  EXPECT_TRUE(FilePath::CurrentDir().IsDir());
  FilePath current_file(FPL(__FILE__));
  EXPECT_FALSE(current_file.IsDir());
  FilePath inexistent_path(FPL("/inexistent/path/"));
  EXPECT_FALSE(inexistent_path.IsDir());
}

TEST(FilePath, IsFile) {
  EXPECT_FALSE(FilePath::CurrentDir().IsFile());
  FilePath current_file(FPL(__FILE__));
  EXPECT_TRUE(current_file.IsFile());
  FilePath inexistent_path(FPL("/inexistent/path"));
  EXPECT_FALSE(inexistent_path.IsFile());
}

TEST(FilePath, GetDirContents) {
  std::vector<FilePath> contents;
  const FilePath invalid_path(FPL("/invalid/path"));
  invalid_path.GetDirContents(&contents);
  EXPECT_TRUE(contents.empty());

  const FilePath dir_path(FilePath::CurrentDir().Append(FPL("base")));
  dir_path.GetDirContents(&contents);
  FilePath this_file(FPL(__FILE__));
  this_file = this_file.BaseName();

  bool found = false;
  for (size_t i = 0; i < contents.size(); ++i) {
    EXPECT_NE(FilePath::CurrentDir().value(), contents[i].value());
    EXPECT_NE(FilePath::ParentDir().value(), contents[i].value());
    if (contents[i].BaseName().value() == this_file.value()) {
      found = true;
      this_file = contents[i];
    }
  }
  EXPECT_TRUE(found);

  contents.clear();
  this_file.GetDirContents(&contents);
  EXPECT_TRUE(contents.empty());
}

TEST(FilePath, RecursivelyDeleteDir) {
  const mode_t permissions = S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH;
  ScopedTempDir temp_dir("TestRecursivelyDeleteDir");
  ASSERT_TRUE(temp_dir.Create());
  std::vector<FilePath> created_paths;

  FilePath test_dir(temp_dir.Get().Append(FPL("test_dir")));
  int result = mkdir(test_dir.value().c_str(), permissions);
  ELOG_IF(ERROR, result) << "Cannot create test dir " << test_dir.value();
  ASSERT_EQ(0, result);
  created_paths.push_back(test_dir);

  FilePath file_in_test_dir(test_dir.Append(FPL("file01")));
  std::ofstream file_in_test_dir_stream(file_in_test_dir.value().c_str());
  file_in_test_dir_stream.close();
  created_paths.push_back(file_in_test_dir);

  FilePath test_subdir(test_dir.Append(FPL("test_subdir")));
  result = mkdir(test_subdir.value().c_str(), permissions);
  ELOG_IF(ERROR, result) << "Cannot create subdir " << test_subdir.value();
  ASSERT_EQ(0, result);
  created_paths.push_back(test_subdir);

  FilePath file_in_test_subdir(test_subdir.Append(FPL("file02")));
  std::ofstream file_in_test_subdir_stream(file_in_test_subdir.value().c_str());
  file_in_test_subdir_stream.close();
  created_paths.push_back(file_in_test_subdir);

  FilePath symlink_in_test_subdir(test_subdir.Append(FPL("symlink")));
  result = symlink(file_in_test_dir.value().c_str(),
                   symlink_in_test_subdir.value().c_str());
  ELOG_IF(ERROR, result) << "Cannot create symlink "
                         << symlink_in_test_subdir.value();
  ASSERT_EQ(0, result);
  created_paths.push_back(symlink_in_test_subdir);

  for (size_t i = 0; i < created_paths.size(); ++i) {
    EXPECT_TRUE(created_paths[i].Exists()) << created_paths[i].value();
  }
  RecursivelyDeleteDir(test_dir);
  for (size_t i = 0; i < created_paths.size(); ++i) {
    EXPECT_FALSE(created_paths[i].Exists()) << created_paths[i].value();
  }
}

}  // anonymous namespace
}  // namespace base
