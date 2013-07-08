// Copyright (c) 2013 Cristian Patrasciuc. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "base/command_line.h"

#include <algorithm>
#include <iterator>
#include <map>
#include <sstream>
#include <string>
#include <utility>
#include <vector>

#include "base/log.h"
#include "base/string_util.h"

namespace base {

typedef std::map<std::string, std::string>::iterator SwitchIterator;
typedef std::map<std::string, std::string>::const_iterator ConstSwitchIterator;

namespace {

bool IsSwitch(const std::string& arg) {
  size_t index = 0;
  while (index < arg.size() && arg[index] == '-') {
    ++index;
  }
  if (index >= arg.size()) {
    return false;
  }
  return (index == 1) || (index == 2);
}

}  // anonymous namespace

CommandLine::CommandLine(const std::string& program_name)
    : switches_(),
      args_(),
      program_name_(program_name),
      is_initialized_(false) {
}

void CommandLine::Init(int argc, const char* const* argv) {
  Init(std::vector<std::string>(argv, argv + argc));
}

void CommandLine::Init(const std::vector<std::string>& args) {
  DCHECK(!is_initialized_);
  for (size_t i = 0; i < args.size(); ++i) {
    std::string switch_name(args[i]);
    std::string switch_value;
    const size_t pos = args[i].find('=');
    if (pos != std::string::npos) {
      switch_name = args[i].substr(0, pos);
      switch_value = args[i].substr(pos + 1);
    }
    if (IsSwitch(switch_name)) {
      AppendSwitch(switch_name, switch_value, true);
    } else if (i == 0 && program_name_.empty()) {
      program_name_ = args[i];
    } else {
      args_.push_back(args[i]);
    }
  }
  is_initialized_ = true;
}

void CommandLine::AppendSwitch(const std::string& switch_name,
                               const std::string& switch_value,
                               bool overwrite) {
  DCHECK(IsSwitch(switch_name));
  DCHECK_EQ(std::string::npos, switch_name.find('='));
  is_initialized_ = true;
  std::pair<SwitchIterator, bool> it =
      switches_.insert(std::make_pair(switch_name, switch_value));
  if (!it.second && overwrite) {
    it.first->second = switch_value;
  }
}

void CommandLine::AppendArgument(const std::string& arg) {
  is_initialized_ = true;
  args_.push_back(arg);
}

bool CommandLine::HasSwitch(const std::string& switch_name) const {
  return switches_.count(switch_name) != 0;
}

std::string CommandLine::GetSwitchValue(const std::string& switch_name) const {
  DCHECK(HasSwitch(switch_name));
  return switches_.find(switch_name)->second;
}

std::string CommandLine::GetAsString() const {
  std::ostringstream result;
  if (!program_name_.empty()) {
    result << program_name_ << " ";
  }
  for (ConstSwitchIterator it = switches_.begin();
      it != switches_.end(); ++it) {
    result << it->first;
    if (!it->second.empty()) {
      result << "=" << it->second;
    }
    result << " ";
  }
  std::copy(args_.begin(), args_.end(),
      std::ostream_iterator<std::string>(result, " "));
  return StripString(result.str());
}

// TODO(command_line): Use singleton here?
// static
CommandLine* CommandLine::ForCurrentProcess() {
  if (!current_process_command_line_) {
    current_process_command_line_ = new CommandLine;
  }
  return current_process_command_line_;
}

// static
void CommandLine::DeleteForCurrentProcess() {
  if (current_process_command_line_) {
    delete current_process_command_line_;
    current_process_command_line_ = NULL;
  }
}

// static
CommandLine* CommandLine::current_process_command_line_ = NULL;

}  // namespace base
