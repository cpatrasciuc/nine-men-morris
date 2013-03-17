// Copyright (c) 2012 Cristian Patrasciuc. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "base/file_path.h"

#include <dirent.h>
#include <fcntl.h>
#include <libgen.h>
#include <linux/limits.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <unistd.h>

#include <string>
#include <vector>

#include "base/log.h"
#include "base/ptr/scoped_malloc_ptr.h"

namespace {

bool CheckFileType(const char* path, int mask) {
  struct stat stat_info;
  int result = lstat(path, &stat_info);
  if (result == -1) {
    ELOG(ERROR) << "Could not get file info for " << path;
    return false;
  }
  return (stat_info.st_mode & mask) != 0;
}

}  // anonymous namespace

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
  return IsDir() || IsFile();
}

FilePath FilePath::RealPath() const {
  char actual_path[PATH_MAX + 1];
  const char* result = realpath(path_.c_str(), actual_path);
  if (!result) {
    ELOG(ERROR) << "Could not obtain the real path of " << path_;
    return FilePath();
  }
  actual_path[PATH_MAX] = '\0';
  return FilePath(actual_path);
}

bool FilePath::IsDir() const {
  return CheckFileType(path_.c_str(), S_IFDIR);
}

bool FilePath::IsFile() const {
  return CheckFileType(path_.c_str(), S_IFREG);
}

void FilePath::GetDirContents(std::vector<FilePath>* contents) const {
  DCHECK(contents);
  if (!IsDir()) {
    return;
  }
  const FilePath absolute_path(RealPath());
  DIR* dir_ptr = opendir(absolute_path.value().c_str());
  if (!dir_ptr) {
    ELOG(ERROR) << "Could not open directory " << path_;
  }
  struct dirent* dir_entry = NULL;
  // According to this blog entry, readdir() is safe enough, so we ignore the
  // cppline.py error.
  // http://elliotth.blogspot.ro/2012/10/how-not-to-use-readdirr3.html
  while ((dir_entry = readdir(dir_ptr))) {  // NOLINT(runtime/threadsafe_fn)
    if (CurrentDir().value() == dir_entry->d_name) {
      continue;
    }
    if (ParentDir().value() == dir_entry->d_name) {
      continue;
    }
    contents->push_back(absolute_path.Append(dir_entry->d_name));
  }
  closedir(dir_ptr);
}

void FilePath::Delete() const {
  if (Exists()) {
    const int result = IsDir() ? rmdir(path_.c_str()) : unlink(path_.c_str());
    if (result) {
      ELOG(ERROR) << "Could not delete " << path_;
    }
  }
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

void RecursivelyDeleteDir(const FilePath& dir_path) {
  DCHECK(dir_path.IsDir());
  std::vector<FilePath> contents;
  dir_path.GetDirContents(&contents);
  for (size_t i = 0; i < contents.size(); ++i) {
    if (contents[i].IsDir()) {
      RecursivelyDeleteDir(contents[i]);
    } else {
      contents[i].Delete();
    }
  }
  dir_path.Delete();
}

}  // namespace base
