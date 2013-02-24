// Copyright (c) 2013 Cristian Patrasciuc. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "base/file_util.h"

#include <unistd.h>

#include <cstring>
#include <cstdlib>
#include <string>

#include "base/file_path.h"
#include "base/log.h"

namespace base {

const char kTempDirSuffix[] = "XXXXXX";

ScopedTempDir::ScopedTempDir(const std::string& name_prefix)
    : name_prefix_(name_prefix), path_(), created_(false) {}

ScopedTempDir::~ScopedTempDir() {
  RecursivelyDeleteDir(path_);
}

const FilePath& ScopedTempDir::Get() const {
  DCHECK(created_);
  return path_;
}

bool ScopedTempDir::Create() {
  DCHECK(!created_);

  const char* tmp_dir = std::getenv("TMPDIR");
  if (!tmp_dir) {
    tmp_dir = "/tmp";
  }
  if (access(tmp_dir, R_OK | W_OK)) {
    LOG(ERROR) << "Could not find a valid base folder: " << tmp_dir;
    return false;
  }
  const size_t suffix_length = std::strlen(kTempDirSuffix);

  const bool should_append_suffix = name_prefix_.size() < suffix_length ||
      name_prefix_.find(kTempDirSuffix) != name_prefix_.size() - suffix_length;

  FilePath dir_name;
  if (should_append_suffix) {
    dir_name = FilePath(name_prefix_ + kTempDirSuffix);
  } else {
    dir_name = FilePath(name_prefix_);
  }

  const FilePath full_path = FilePath(tmp_dir).Append(dir_name);
  char* path = const_cast<char*>(full_path.value().c_str());
  path = mkdtemp(path);
  if (!path) {
    ELOG(ERROR) << "Could not create temporary folder";
    return false;
  }
  created_ = true;
  path_ = FilePath(path);
  return true;
}

}  // namespace base
