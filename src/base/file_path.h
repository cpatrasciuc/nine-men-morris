// Copyright (c) 2012 Cristian Patrasciuc. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef BASE_FILE_PATH_H_
#define BASE_FILE_PATH_H_

#include <string>

#include "base/base_export.h"

namespace base {

class BASE_EXPORT FilePath {
 public:
  // The data type used to store a file path.
  // These might be changed in the future if the project will be ported on
  // Windows so that we also support internationalized file paths.
  typedef std::string StringType;
  typedef std::string::value_type CharType;

  FilePath();
  explicit FilePath(const StringType& path);

  StringType value() const {
    return path_;
  }

  bool empty() const {
    return path_.empty();
  }

  // If |this| is a folder, it returns the path to the parent folder. If the
  // path is relative and the parent folder is not present, the result will be
  // equal to |CurrentDir()|. If |this| is the root folder, it returns the root.
  // If |this| is a file, returns the path to the folder that contains the file.
  FilePath DirName() const;

  // Returns a FilePath containing only the file or folder name of |this|.
  // If the path is empty the result will be equal to |CurrentDir()|.
  FilePath BaseName() const;

  // Returns true if the path represented by this objects is an absolute path;
  // if the path is a relative path, it returns false.
  bool IsAbsolute() const;

  // If |this| refers to a file, it returns the file extension. Otherwise, it
  // returns an empty path.
  FilePath Extension() const;

  // Adds the given extension to the file path refered by |this|. If |this| is
  // empty or points to a folder, the method returns an empty file path. If the
  // extension is empty, the method returns a file path equal to |this|.
  FilePath AddExtension(const FilePath& extension) const;

  // Removes unnecessary separator at the end of |this| file path.
  FilePath StripTrailingSeparators() const;

  // If |file_path| is absolute, the result will be equal to it. If |file_path|
  // is relative, it is appended to the path referred by |this|. Separators will
  // be inserted where needed.
  FilePath Append(FilePath file_path) const;
  FilePath Append(StringType file_path_str) const;

  // Returns true if the file or folder referred by |this| exists on the file
  // system. If an error occurs while trying to verify this, the method assumes
  // that the path does not exist and returns false.
  bool Exists() const;

  // Returns a file path that represents the current directory (i.e. ".").
  static FilePath CurrentDir();

  // Returns a file path that represents the parent directory (i.e. "..").
  static FilePath ParentDir();

  // The file path separator for the current platform (e.g. '/' on Linux)
  static const CharType separator;

 private:
  StringType path_;
};

// This macro should be used when hard coding file paths in the code. On Linux
// it has no effect. On Windows it will convert it to a wide string.
#define FILE_PATH_LITERAL(x) x

}  // namespace base

#endif  // BASE_FILE_PATH_H_

