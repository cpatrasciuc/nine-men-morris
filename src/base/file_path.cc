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

const FilePath::CharType kCurrentDirString[] = FILE_PATH_LITERAL(".");
const FilePath::CharType kParentDirString[] = FILE_PATH_LITERAL("..");
const FilePath::CharType kExtensionSeparator[] = FILE_PATH_LITERAL(".");

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
  const StringType base_name_str = BaseName().value();
  StringType::size_type extension_start = -1;
  StringType::size_type pos = extension_start;
  while ((pos = base_name_str.find(kExtensionSeparator, extension_start + 1)) !=
         StringType::npos) {
    extension_start = pos;
  };
  if (extension_start == StringType::size_type(-1)) {
    return FilePath();
  }
  return FilePath(base_name_str.substr(extension_start));
}

FilePath FilePath::AddExtension(const FilePath& extension) const {
  if (empty() ||
      path_[path_.size() - 1] == FilePath::separator ||
      path_ == kCurrentDirString ||
      path_ == kParentDirString) {
    return FilePath();
  }
  if (extension.empty() || extension.value() == kExtensionSeparator) {
    return *this;
  }
  StringType new_path(path_);
  if (extension.value().find(kExtensionSeparator) != 0) {
    new_path.append(kExtensionSeparator);
  }
  new_path.append(extension.value());
  return FilePath(new_path);
}

FilePath FilePath::StripTrailingSeparators() const {
  size_t pos = path_.find_last_not_of(FilePath::separator);
  if (pos == StringType::npos) {
    pos = 0;
  }
  return FilePath(path_.substr(0, pos + 1));
}

FilePath FilePath::Append(const FilePath& file_path) const {
  if (file_path.IsAbsolute()) {
    return file_path;
  }
  StringType result = StripTrailingSeparators().value();
  if (!file_path.empty()) {
    if (!result.empty() && result[result.size() - 1] != FilePath::separator) {
      result.push_back(FilePath::separator);
    }
    result.append(file_path.value());
  }
  return FilePath(result);
}

FilePath FilePath::Append(const StringType& file_path_str) const {
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