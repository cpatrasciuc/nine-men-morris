// Copyright (c) 2013 Cristian Patrasciuc. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "console_game/board_renderer.h"

#include <iomanip>
#include <sstream>
#include <string>
#include <vector>

#include "base/console.h"
#include "game/board.h"
#include "game/board_location.h"
#include "game/piece_color.h"

namespace console_game {

BoardRenderer::BoardRenderer(const game::Board& board, Delegate* delegate)
    : board_(board), delegate_(delegate) {
  if (!delegate) {
    Reset(delegate_, new ConsoleRenderDelegate);
  }
}

// TODO(console_game): Add detail explanation about this Draw() method
void BoardRenderer::Render() {
  // Multipliers for lines and columns
  const int lmul = 3;
  const int cmul = 5;

  const int lines = lmul * board_.size() - 1;
  const int cols = cmul * board_.size() - 1;

  // Store whether we found the first box on the given line/column
  std::vector<bool> found_by_line(board_.size(), false);
  std::vector<bool> found_by_col(board_.size(), false);

  const int indent = 5;
  delegate_->Print(std::string(indent, ' '));
  for (int col = 0; col < cols; ++col) {
    char column_name(' ');
    if (col % cmul == 1) {
      column_name = 'A' + (col / cmul);
    }
    delegate_->Print(std::string(1, column_name));
  }
  delegate_->Print("\n\n");

  const bool is_three_men_morris(board_.size() == 3);

  for (int line = 0; line < lines; ++line) {
    std::ostringstream line_name;
    if (line % lmul == 1) {
      line_name << std::setw(indent - 1) << line / lmul + 1 << " ";
    } else {
      line_name << std::string(indent, ' ');
    }
    delegate_->Print(line_name.str());

    for (int col = 0; col < cols; ++col) {
      // Check if the current coordinates correspond to a valid board location
      if (line % lmul != lmul - 1 && col % cmul < cmul - 2) {
        game::BoardLocation loc(line / lmul, col / cmul);
        if (board_.IsValidLocation(loc)) {
          game::PieceColor board_color = board_.GetPieceAt(loc);
          delegate_->PrintFillChar(FILL, board_color);
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
        while (i < board_.size()) {
          game::BoardLocation loc(line / lmul, i);
          if (board_.IsValidLocation(loc)) {
            horizontal = true;
            break;
          }
          ++i;
        }
        if (line / lmul == board_.size() / 2 &&
            i == board_.size() / 2 + 1) {
          horizontal = is_three_men_morris;
        }
      }
      if (horizontal) {
        delegate_->PrintFillChar(HORIZONTAL_LINE);
        continue;
      }

      // Since it is not a valid board location nor a horizontal line, check if
      // the current coordinates are situated on a vertical line connecting two
      // adjacent board locations.
      bool vertical = false;
      if (col % cmul == 1 && found_by_col[col / cmul]) {
        int i = line / lmul + 1;
        while (i < board_.size()) {
          game::BoardLocation loc(i, col / cmul);
          if (board_.IsValidLocation(loc)) {
            vertical = true;
            break;
          }
          ++i;
        }
        if (col / cmul == board_.size() / 2 &&
            i == board_.size() / 2 + 1) {
          vertical = is_three_men_morris;
        }
      }
      if (vertical) {
        delegate_->PrintFillChar(VERTICAL_LINE);
        continue;
      }

      // If we reached this point it means that no special character should be
      // printed for the current coordinates.
      delegate_->PrintFillChar(EMPTY);
    }
    delegate_->Print("\n");
  }
}

BoardRenderer::Delegate::Delegate() {}

BoardRenderer::Delegate::~Delegate() {}

ConsoleRenderDelegate::ConsoleRenderDelegate() {}

ConsoleRenderDelegate::~ConsoleRenderDelegate() {}

void ConsoleRenderDelegate::Print(const std::string& str) {
  std::cout << str;
  std::cout.flush();
}

void ConsoleRenderDelegate::PrintFillChar(BoardRenderer::FillType fill_type,
                                          game::PieceColor color) {
  base::Console::Color output_color(base::Console::COLOR_DEFAULT);
  switch (color) {
    case game::BLACK_COLOR:
      output_color = base::Console::COLOR_GREEN;
      break;
    case game::WHITE_COLOR:
      output_color = base::Console::COLOR_RED;
      break;
    case game::NO_COLOR:
      output_color = base::Console::COLOR_DEFAULT;
      break;
  }

  std::string str;
  switch (fill_type) {
    case BoardRenderer::EMPTY:
      str = " ";
      break;
    case BoardRenderer::VERTICAL_LINE:
      str = "|";
      break;
    case BoardRenderer::HORIZONTAL_LINE:
      str = "_";
      break;
    case BoardRenderer::FILL:
      // http://www.fileformat.info/info/unicode/block/block_elements/images.htm
      str = (color == game::NO_COLOR) ? "\xe2\x96\x91" : "\xe2\x96\x93";
      break;
  }

  base::Console::ColoredPrintf(output_color, "%s", str.c_str());
}

}  // namespace console_game
