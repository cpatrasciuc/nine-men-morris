// Copyright (c) 2013 Cristian Patrasciuc. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "console_game/help_command_handler.h"

#include <iostream>
#include <map>
#include <ostream>
#include <string>
#include <vector>

#include "base/console.h"
#include "base/log.h"
#include "game/game.h"

namespace console_game {

HelpCommandHandler::HelpCommandHandler(
    const std::map<std::string, CommandHandler*>& handlers, std::ostream* out)
    : handlers_(handlers), out_(out) {
  DCHECK(out);
}

HelpCommandHandler::~HelpCommandHandler() {}

std::vector<std::string> HelpCommandHandler::SupportedCommandTypes() const {
  std::vector<std::string> result;
  result.push_back("help");
  result.push_back("?");
  return result;
}

bool HelpCommandHandler::ProcessCommand(const std::string& command_type,
                                        const std::string& args,
                                        game::Game* game_model) {
  if (IsInteractive()) {
    base::Console::ClearScreen();
  }
  std::map<std::string, CommandHandler*>::const_iterator it;
  for (it = handlers_.begin(); it != handlers_.end(); ++it) {
    std::string format;
    std::string usage;
    it->second->GetHelpMessage(it->first, &format, &usage);
    DCHECK(!format.empty());
    DCHECK(!usage.empty());
    (*out_) << "  " << format << std::endl;
    (*out_) << "    " << usage << std::endl;
    (*out_) << std::endl;
  }
  if (IsInteractive()) {
    std::cout << "Press <Enter> to continue ...";
    std::cin.get();
  }
  return true;
}

void HelpCommandHandler::GetHelpMessage(const std::string& command_type,
                                        std::string* format,
                                        std::string* usage) {
  *format = command_type;
  *usage = "Displays the help message.";
}

bool HelpCommandHandler::IsInteractive() const {
  return out_ == &std::cout;
}

}  // namespace console_game
