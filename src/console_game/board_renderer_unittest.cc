// Copyright (c) 2013 Cristian Patrasciuc. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include <sstream>

#include "base/basic_macros.h"
#include "console_game/board_renderer.h"
#include "game/board.h"
#include "game/board_location.h"
#include "game/game_type.h"
#include "game/piece_color.h"
#include "gtest/gtest.h"

namespace console_game {
namespace {

class TestRenderDelegate : public BoardRenderer::Delegate {
 public:
  explicit TestRenderDelegate(std::ostringstream* out) : out_(out) {}
  virtual ~TestRenderDelegate() {}

  // BoardRenderer::Delegate interface
  virtual void Print(const std::string& str) {
    (*out_) << str;
    out_->flush();
  }

  virtual void PrintFillChar(BoardRenderer::FillType fill_type,
                             game::PieceColor color = game::NO_COLOR) {
    char c = '?';
    switch (fill_type) {
      case BoardRenderer::EMPTY:
        c = 'E';
        break;
      case BoardRenderer::VERTICAL_LINE:
        c = '|';
        break;
      case BoardRenderer::HORIZONTAL_LINE:
        c = '_';
        break;
      case BoardRenderer::FILL:
        switch (color) {
          case game::BLACK_COLOR:
            c = 'B';
            break;
          case game::WHITE_COLOR:
            c = 'W';
            break;
          case game::NO_COLOR:
            c = 'N';
        }
        break;
    }
    out_->put(c);
  }

 private:
  std::ostringstream* out_;
};

TEST(BoardRenderer, BoardRenderer) {
  const char* const expected_output[] = {
      // Nine men morris board
      "      A    B    C    D    E    F    G  \n"
      "\n"
      "     WWW____________NNN____________NNNE\n"
      "   1 WWWEEEEEEEEEEEENNNEEEEEEEEEEEENNNE\n"
      "     E|EEEEEEEEEEEEEE|EEEEEEEEEEEEEE|EE\n"
      "     E|EEENNN_______NNN_______NNNEEE|EE\n"
      "   2 E|EEENNNEEEEEEENNNEEEEEEENNNEEE|EE\n"
      "     E|EEEE|EEEEEEEEE|EEEEEEEEE|EEEE|EE\n"
      "     E|EEEE|EEENNN__NNN__NNNEEE|EEEE|EE\n"
      "   3 E|EEEE|EEENNNEENNNEENNNEEE|EEEE|EE\n"
      "     E|EEEE|EEEE|EEEEEEEEE|EEEE|EEEE|EE\n"
      "     NNN__NNN__NNNEEEEEEENNN__NNN__NNNE\n"
      "   4 NNNEENNNEENNNEEEEEEENNNEENNNEENNNE\n"
      "     E|EEEE|EEEE|EEEEEEEEE|EEEE|EEEE|EE\n"
      "     E|EEEE|EEENNN__NNN__NNNEEE|EEEE|EE\n"
      "   5 E|EEEE|EEENNNEENNNEENNNEEE|EEEE|EE\n"
      "     E|EEEE|EEEEEEEEE|EEEEEEEEE|EEEE|EE\n"
      "     E|EEENNN_______NNN_______NNNEEE|EE\n"
      "   6 E|EEENNNEEEEEEENNNEEEEEEENNNEEE|EE\n"
      "     E|EEEEEEEEEEEEEE|EEEEEEEEEEEEEE|EE\n"
      "     NNN____________NNN____________BBBE\n"
      "   7 NNNEEEEEEEEEEEENNNEEEEEEEEEEEEBBBE\n",

      // Six men morris board
      "      A    B    C    D    E  \n"
      "\n"
      "     WWW_______NNN_______NNNE\n"
      "   1 WWWEEEEEEENNNEEEEEEENNNE\n"
      "     E|EEEEEEEEE|EEEEEEEEE|EE\n"
      "     E|EEENNN__NNN__NNNEEE|EE\n"
      "   2 E|EEENNNEENNNEENNNEEE|EE\n"
      "     E|EEEE|EEEEEEEEE|EEEE|EE\n"
      "     NNN__NNNEEEEEEENNN__NNNE\n"
      "   3 NNNEENNNEEEEEEENNNEENNNE\n"
      "     E|EEEE|EEEEEEEEE|EEEE|EE\n"
      "     E|EEENNN__NNN__NNNEEE|EE\n"
      "   4 E|EEENNNEENNNEENNNEEE|EE\n"
      "     E|EEEEEEEEE|EEEEEEEEE|EE\n"
      "     NNN_______NNN_______BBBE\n"
      "   5 NNNEEEEEEENNNEEEEEEEBBBE\n",

      // Three men morris board
      "      A    B    C  \n"
      "\n"
      "     WWW__NNN__NNNE\n"
      "   1 WWWEENNNEENNNE\n"
      "     E|EEEE|EEEE|EE\n"
      "     NNN__NNN__NNNE\n"
      "   2 NNNEENNNEENNNE\n"
      "     E|EEEE|EEEE|EE\n"
      "     NNN__NNN__BBBE\n"
      "   3 NNNEENNNEEBBBE\n"
  };

  const game::GameType game_types[] = {
    game::NINE_MEN_MORRIS,
    game::SIX_MEN_MORRIS,
    game::THREE_MEN_MORRIS
  };

  ASSERT_EQ(arraysize(game_types), arraysize(expected_output));

  for (size_t i = 0; i < arraysize(game_types); ++i) {
    game::Board board(game_types[i]);
    board.AddPiece(game::BoardLocation(0, 0), game::WHITE_COLOR);
    board.AddPiece(game::BoardLocation(board.size() - 1, board.size() - 1),
                   game::BLACK_COLOR);
    std::ostringstream out;
    BoardRenderer renderer(board, new TestRenderDelegate(&out));
    renderer.Render();
    EXPECT_EQ(expected_output[i], out.str());
  }
}

}  // anonymous namespace
}  // namespace console_game
