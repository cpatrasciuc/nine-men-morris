// Copyright (c) 2013 Cristian Patrasciuc. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef CONSOLE_GAME_BOARD_RENDERER_H_
#define CONSOLE_GAME_BOARD_RENDERER_H_

#include <string>

#include "base/basic_macros.h"
#include "base/ptr/scoped_ptr.h"
#include "console_game/console_game_export.h"
#include "game/piece_color.h"

namespace game {
class Board;
}

namespace console_game {

// This class is responsible for drawing the game board. It performs all the
// necessary logic and delegates the actual output to a Delegate instance.
// This object is responsible for printing the given strings and characters.
// For more details see ConsoleRenderDelegate.
class CONSOLE_GAME_EXPORT BoardRenderer {
 public:
  // Enum used to represent different spots during the rendering of a board.
  enum FillType {
    // A spot situated on a vertical line connecting to adjacent locations
    VERTICAL_LINE,
    // A spot situated on a horizontal line connecting to adjacent locations
    HORIZONTAL_LINE,
    // A spot is situated on a board location
    FILL,
    // None of the above
    EMPTY
  };

  class Delegate {
   public:
    Delegate();
    virtual ~Delegate();

    // The delegate should print the given string. It is not part of the board.
    // It is either part of line/column headers or it's used to skip to a new
    // line.
    virtual void Print(const std::string& str) = 0;

    // The next char that is to be printed is part of the board. Its meaning
    // is given by the |fill_type| argument. If the character represents a valid
    // board location, the |color| argument is used to indicate that the
    // location is occupied by a given player or not.
    virtual void PrintFillChar(FillType fill_type,
                               game::PieceColor color = game::NO_COLOR) = 0;

   private:
    DISALLOW_COPY_AND_ASSIGN(Delegate);
  };

  // If |delegate| is NULL, an instance of ConsoleRenderDelegate will be used
  explicit BoardRenderer(const game::Board& board,
                         Delegate* delegate = NULL);

  // Draw the current state of the board
  void Render();

 private:
  // The board that is to be rendered
  const game::Board& board_;

  // Pointer to the function used to do the actual drawing
  base::ptr::scoped_ptr<Delegate> delegate_;

  DISALLOW_COPY_AND_ASSIGN(BoardRenderer);
};

class CONSOLE_GAME_EXPORT ConsoleRenderDelegate
    : public BoardRenderer::Delegate {
 public:
  ConsoleRenderDelegate();
  virtual ~ConsoleRenderDelegate();

  // BoardRenderer::Delegate interface
  virtual void Print(const std::string& str);
  virtual void PrintFillChar(BoardRenderer::FillType fill_type,
                             game::PieceColor color = game::NO_COLOR);
};

}  // namespace console_game

#endif  // CONSOLE_GAME_BOARD_RENDERER_H_

