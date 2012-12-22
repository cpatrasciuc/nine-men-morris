// Copyright (c) 2012 Cristian Patrasciuc. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "base/file_path.h"

#include <libgen.h>
#include <string.h>
#include <unistd.h>

#include "base/log.h"
#include "base/ptr/scoped_malloc_ptr.h"

namespace base {

const FilePath::CharType kCurrentDirString[] = ".";
const FilePath::CharType kParentDirString[] = "..";

FilePath::FilePath() : path_() {}

FilePath::FilePath(const StringType& path) : path_(path) {}

FilePath FilePath::DirName() const {
  base::ptr::scoped_malloc_ptr<CharType> path_copy(strdup(path_.c_str()));
  return FilePath(dirname(Get(path_copy)));
}

FilePath FilePath::BaseName() const {
  base::ptr::scoped_malloc_ptr<CharType> path_copy(strdup(path_.c_str()));
  return FilePath(basename(Get(path_copy)));
}

bool FilePath::IsAbsolute() const {
  return (!empty()) && (path_[0] == FilePath::separator);
}

FilePath FilePath::Extension() const {
  if (empty() || (path_[path_.size() - 1] == FilePath::separator)) {
    return FilePath();
  }
  const FilePath base_name = BaseName();
  const size_t dot_position = base_name.value().find_last_of('.');
  if (dot_position == StringType::npos) {
    return FilePath();
  }
  return FilePath(base_name.value().substr(dot_position));
}

FilePath FilePath::StripTrailingSeparators() const {
  size_t pos = path_.find_last_not_of(FilePath::separator);
  if (pos == StringType::npos) {
    pos = 0;
  }
  return FilePath(path_.substr(0, pos + 1));
}

FilePath FilePath::Append(FilePath file_path) const {
  if (file_path.IsAbsolute()) {
    return file_path;
  }
  StringType result = StripTrailingSeparators().value();
  if (!file_path.empty()) {
    if (!result.empty() && result[result.size() - 1] != '/') {
      result.push_back(FilePath::separator);
    }
    result.append(file_path.value());
  }
  return FilePath(result);
}

FilePath FilePath::Append(StringType file_path_str) const {
  return Append(FilePath(file_path_str));
}

bool FilePath::Exists() const {
  const int result = access(path_.c_str(), F_OK);
  if (result == -1) {
    ELOG(ERROR) << "Existence test for " << path_;
  }
  return result != -1;
}

// static
FilePath FilePath::CurrentDir() {
  return FilePath(kCurrentDirString);
}

// static
FilePath FilePath::ParentDir() {
  return FilePath(kParentDirString);
}

// static
const FilePath::CharType FilePath::separator = '/';

}  // namespace base
