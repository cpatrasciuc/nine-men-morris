// Copyright (c) 2012 Cristian Patrasciuc. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "console_game/console_game.h"

#include <string>
#include <vector>

#include "base/console.h"
#include "game/board_location.h"
#include "game/game_options.h"

namespace console_game {

namespace {

enum CharType {
  EMPTY,
  VERTICAL_LINE,
  HORIZONTAL_LINE,
  FILL
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
    case FILL:
      c = 176;
      break;
  }
  base::Console::ColoredPrintf(color, "%c", c);
}

}  // anonymous namespace

ConsoleGame::ConsoleGame(const game::GameOptions& options) : game_(options) {}

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

  for (int line = 0; line < lines; ++line) {
    for (int col = 0; col < cols; ++col) {
      // Check if the current coordinates correspond to a valid board location
      if (line % lmul != lmul - 1 && col % cmul < cmul - 2) {
        game::BoardLocation loc(line / lmul, col / cmul);
        if (game_.board().IsValidLocation(loc)) {
          PrintColoredFillChar(base::Console::COLOR_BLACK, FILL);
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
          horizontal = false;
        }
      }
      if (horizontal) {
        PrintColoredFillChar(base::Console::COLOR_BLACK, HORIZONTAL_LINE);
        continue;
      }

      // Since it is not a valid board location nor a horizontal line, check if
      // the current coordinates are situated on a vertical line connecting two
      // adjacent board locations.
      bool vertical = false;
      if (col % cmul == 1 && found_by_col[col / cmul]) {
        int i = line / lmul + 1;
        while (i < game_.board().size()) {
          game::BoardLocation loc(i, col / 5);
          if (game_.board().IsValidLocation(loc)) {
            vertical = true;
            break;
          }
          ++i;
        }
        if (col / cmul == game_.board().size() / 2 &&
            i == game_.board().size() / 2 + 1) {
          vertical = false;
        }
      }
      if (vertical) {
        PrintColoredFillChar(base::Console::COLOR_BLACK, VERTICAL_LINE);
        continue;
      }

      // If we reached this point it means that no special character should be
      // printed for the current coordinates.
      PrintColoredFillChar(base::Console::COLOR_BLACK, EMPTY);
    }
    std::cout << std::endl;
  }
}

void ConsoleGame::Run() {
  const int kMaxCommandSize = 256;
  char cmd_buffer[kMaxCommandSize];
  Draw();
  while (std::cin.getline(cmd_buffer, kMaxCommandSize)) {
    std::string command(cmd_buffer);
    if (command == "q" || command == "Q") {
      break;
    }
    Draw();
    std::cout << "Next command or 'q' to quit: ";
  }
}

}  // namespace console_game
