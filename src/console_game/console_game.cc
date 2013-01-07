// Copyright (c) 2012 Cristian Patrasciuc. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "console_game/console_game.h"

#include <iomanip>
#include <map>
#include <memory>
#include <sstream>
#include <string>
#include <vector>

#include "base/console.h"
#include "base/log.h"
#include "base/string_util.h"
#include "console_game/command_handler.h"
#include "game/board_location.h"
#include "game/game_options.h"
#include "game/piece_color.h"
#include "game/player_action.h"

namespace console_game {

namespace {

const char kDefaultCommandHandlerEntry[] = "__default__";

enum CharType {
  EMPTY,
  VERTICAL_LINE,
  HORIZONTAL_LINE,
  PARTIAL_FILL,
  FULL_FILL
};

void PrintColoredFillChar(base::Console::Color color, CharType type) {
  char c = '?';
  switch (type) {
    case EMPTY:
      c = ' ';
      break;
    case VERTICAL_LINE:
      c = '|';
      break;
    case HORIZONTAL_LINE:
      c = '_';
      break;
    case PARTIAL_FILL:
      c = 176;
      break;
    case FULL_FILL:
      c = 178;
      break;
  }
  base::Console::ColoredPrintf(color, "%c", c);
}

}  // anonymous namespace

ConsoleGame::ConsoleGame(const game::GameOptions& options)
    : game_(options),
      command_handlers_() {
  std::auto_ptr<CommandHandler> default_handler(new DefaultCommandHandler);
  AddCommandHandler(kDefaultCommandHandlerEntry, default_handler);
}

ConsoleGame::~ConsoleGame() {
  std::map<std::string, CommandHandler*>::iterator it;
  for (it = command_handlers_.begin(); it != command_handlers_.end(); ++it) {
    delete it->second;
  }
}

void ConsoleGame::AddCommandHandler(const std::string& command_type,
                                    std::auto_ptr<CommandHandler> handler) {
  DCHECK_EQ(command_type.find_first_of(base::kWhiteSpaceChars),
            std::string::npos);
  CommandHandler* handler_ptr(handler.release());
  std::string lowercase_command_type(base::ToLowerCase(command_type));
  command_handlers_.insert(std::make_pair(lowercase_command_type, handler_ptr));
}

// TODO(console_game): Add detail explanation about this Draw() method
void ConsoleGame::Draw() {
  base::Console::ClearScreen();

  // Multipliers for lines and columns
  const int lmul = 3;
  const int cmul = 5;

  const int lines = lmul * game_.board().size() - 1;
  const int cols = cmul * game_.board().size() - 1;

  // Store whether we found the first box on the given line/column
  std::vector<bool> found_by_line(game_.board().size(), false);
  std::vector<bool> found_by_col(game_.board().size(), false);

  const int indent = 5;
  std::cout << std::string(indent, ' ');
  for (int col = 0; col < cols; ++col) {
    if (col % cmul == 1) {
      std::cout.put('A' + (col / cmul));
    } else {
      std::cout.put(' ');
    }
  }
  std::cout << "\n\n";

  const bool is_three_men_morris(
      game_.options().game_type() == game::GameOptions::THREE_MEN_MORRIS);

  for (int line = 0; line < lines; ++line) {
    if (line % lmul == 1) {
      std::cout << std::setw(indent - 1) << line / lmul + 1 << " ";
    } else {
      std::cout << std::string(indent, ' ');
    }
    for (int col = 0; col < cols; ++col) {
      // Check if the current coordinates correspond to a valid board location
      if (line % lmul != lmul - 1 && col % cmul < cmul - 2) {
        game::BoardLocation loc(line / lmul, col / cmul);
        if (game_.board().IsValidLocation(loc)) {
          base::Console::Color text_color = base::Console::COLOR_BLACK;
          game::PieceColor board_color = game_.board().GetPieceAt(loc);
          switch (board_color) {
            case game::WHITE_COLOR:
              text_color = base::Console::COLOR_RED;
              break;
            case game::BLACK_COLOR:
              text_color = base::Console::COLOR_GREEN;
              break;
            case game::NO_COLOR:
              text_color = base::Console::COLOR_DEFAULT;
              break;
          }
          PrintColoredFillChar(text_color,
              board_color == game::NO_COLOR ? PARTIAL_FILL : FULL_FILL);
          found_by_line[line / lmul] = true;
          found_by_col[col / cmul] = true;
          continue;
        }
      }

      // Since it's not a valid board location, check if the current coordinates
      // are situated on a horizontal line connecting two adjacent locations.
      bool horizontal = false;
      if (line % lmul == 0 && found_by_line[line / lmul]) {
        int i = col / cmul + 1;
        while (i < game_.board().size()) {
          game::BoardLocation loc(line / lmul, i);
          if (game_.board().IsValidLocation(loc)) {
            horizontal = true;
            break;
          }
          ++i;
        }
        if (line / lmul == game_.board().size() / 2 &&
            i == game_.board().size() / 2 + 1) {
          horizontal = is_three_men_morris;
        }
      }
      if (horizontal) {
        PrintColoredFillChar(base::Console::COLOR_DEFAULT, HORIZONTAL_LINE);
        continue;
      }

      // Since it is not a valid board location nor a horizontal line, check if
      // the current coordinates are situated on a vertical line connecting two
      // adjacent board locations.
      bool vertical = false;
      if (col % cmul == 1 && found_by_col[col / cmul]) {
        int i = line / lmul + 1;
        while (i < game_.board().size()) {
          game::BoardLocation loc(i, col / cmul);
          if (game_.board().IsValidLocation(loc)) {
            vertical = true;
            break;
          }
          ++i;
        }
        if (col / cmul == game_.board().size() / 2 &&
            i == game_.board().size() / 2 + 1) {
          vertical = is_three_men_morris;
        }
      }
      if (vertical) {
        PrintColoredFillChar(base::Console::COLOR_DEFAULT, VERTICAL_LINE);
        continue;
      }

      // If we reached this point it means that no special character should be
      // printed for the current coordinates.
      PrintColoredFillChar(base::Console::COLOR_DEFAULT, EMPTY);
    }
    std::cout << std::endl;
  }
}

void ConsoleGame::Run() {
  const int kMaxCommandSize = 256;
  char cmd_buffer[kMaxCommandSize] = { 0 };
  game_.Initialize();
  std::string last_command_status("Game started");
  do {
    const std::string command(cmd_buffer);
    if (command == "q" || command == "Q") {
      break;
    }

    // TODO(console_game): Add help command
    if (!command.empty()) {
      const size_t pos = command.find_first_of(base::kWhiteSpaceChars);
      const std::string command_type(base::ToLowerCase(command.substr(0, pos)));
      std::map<std::string, CommandHandler*>::iterator it =
          command_handlers_.find(command_type);
      if (it == command_handlers_.end()) {
        it = command_handlers_.find(kDefaultCommandHandlerEntry);
      }
      DCHECK(it != command_handlers_.end());
      last_command_status = it->second->ProcessCommand(command, &game_);
    }
    Draw();
    std::cout << "\n\n";
    base::Console::ColoredPrintf(base::Console::COLOR_WHITE,
                                 last_command_status.c_str());
    std::cout << "\n\n";
    if (game_.is_game_over()) {
      std::cout << (game_.winner() == game::WHITE_COLOR ? "Red" : "Green")
                << " player wins." << std::endl;
      break;
    }
    std::cout << "Next command or 'q' to quit: ";
    std::cout.flush();
  } while (std::cin.getline(cmd_buffer, kMaxCommandSize));
}

}  // namespace console_game
