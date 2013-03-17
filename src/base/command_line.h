// Copyright (c) 2013 Cristian Patrasciuc. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef BASE_COMMAND_LINE_H_
#define BASE_COMMAND_LINE_H_

#include <map>
#include <string>
#include <vector>

#include "base/base_export.h"
#include "base/basic_macros.h"

namespace base {

class BASE_EXPORT CommandLine {
 public:
  // Initializes an empty CommandLine instance. It also initializes the program
  // name (i.e. the equivalent of argv[0]) if the argument is given.
  explicit CommandLine(const std::string& program_name = "");

  std::string program_name() const { return program_name_; }

  // Appends the switches and arguments to |this| command line. This method can
  // be called only once and must be called before manually adding any switches
  // or arguments. If the first argument in argv/args is not a switch and
  // |program_name_| is empty, the method will initialize |program_name_| with
  // that value.
  // Every argument that starts with a dash or two dashes followed by at least
  // one character other than the equal sign is considered a switch. The value
  // of the switch is everything that follows the first equal sign, if any;
  // otherwise the value is empty.
  void Init(int argc, const char* const* argv);
  void Init(const std::vector<std::string>& args);

  // Add a new switch/value pair to the command line. If |overwrite| is false,
  // and |this| command line already contains a switch with the same name, the
  // value will not be overwritten and the method will be a no-op, even if the
  // previous value of the switch is an empty one.
  // NOTE: |switch_name| must contain the initial dashes too. It cannot contain
  // the equal sign.
  void AppendSwitch(const std::string& switch_name,
                    const std::string& switch_value = "",
                    bool overwrite = true);

  void AppendArgument(const std::string& arg);

  // Checks if |this| command line contains a switch with the given name. The
  // argument must contain the initial dashes too.
  bool HasSwitch(const std::string& switch_name) const;

  // Returns the value of the switch with the given name from |this| command
  // line. |switch_name| must contain the initial dashes too. Also, the caller
  // must ensure that the command line contains a switch with the given name
  // before calling this method (use HasSwitch()).
  std::string GetSwitchValue(const std::string& switch_name) const;

  // Returns the list of "free" arguments (arguments that are not a switches).
  std::vector<std::string> GetArguments() const { return args_; }

  // Constructs and returns the string that represents |this| command line.
  std::string GetAsString() const;

  // Returns a pointer to the global CommandLine instance corresponding to the
  // current process. This is usually initialized in main() and also deleted
  // before main() exits. It must be initialized and deleted only once.
  static CommandLine* ForCurrentProcess();

  // Utility method used to delete the global variable that stores the command
  // line for the current process. It must only be called once, usually before
  // exiting main().
  static void DeleteForCurrentProcess();

 private:
  // Stores the switches and their values.
  std::map<const std::string, std::string> switches_;

  // Stores the list of "free" arguments.
  std::vector<std::string> args_;

  // The executable name (i.e. the equivalent of argv[0]).
  std::string program_name_;

  // Utility variable used to check against calling Init() twice.
  bool is_initialized_;

  // Global CommandLine instance corresponding to the current process, used by
  // ForCurrentProcess(). It is only initialized when ForCurrentProcess() is
  // called for the first time.
  static CommandLine* current_process_command_line_;

  DISALLOW_COPY_AND_ASSIGN(CommandLine);
};

}  // namespace base

#endif  // BASE_COMMAND_LINE_H_

