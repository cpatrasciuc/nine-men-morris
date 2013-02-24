// Copyright (c) 2013 Cristian Patrasciuc. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef BASE_FILE_UTIL_H_
#define BASE_FILE_UTIL_H_

#include <string>

#include "base/base_export.h"
#include "base/basic_macros.h"
#include "base/file_path.h"

namespace base {

// Utility class that can be used to create a temporary directory that is
// automatically deleted from the file system when the |ScopedTempDir| instance
// goes out of scope. The directory location and name is automatically generated
// such that is does not already exist. A prefix can be provided for usability.
class BASE_EXPORT ScopedTempDir {
 public:
  explicit ScopedTempDir(const std::string& name_prefix);
  ~ScopedTempDir();

  // After the temporary folder is created, this method returns its full path.
  const FilePath& Get() const;

  // Try to create the temporary directory. Returns true if the operation
  // completed successfully.
  bool Create();

 private:
  const std::string name_prefix_;
  FilePath path_;
  bool created_;

  DISALLOW_COPY_AND_ASSIGN(ScopedTempDir);
};

}  // namespace base

#endif  // BASE_FILE_UTIL_H_

