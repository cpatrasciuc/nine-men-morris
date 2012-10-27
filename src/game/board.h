// Copyright (c) 2012 Cristian Patrasciuc. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef GAME_BOARD_H_
#define GAME_BOARD_H_

#include <map>
#include <vector>

#include "base/basic_macros.h"
#include "game/board_location.h"
#include "game/game_export.h"

namespace game {

class GAME_EXPORT Board {
 public:
  // This enum is used to describe the state of each location on the board. It
  // contains one value for each player (white and black) and an additional
  // value to designate empty spots.
  enum PieceColor {
    NO_COLOR,
    WHITE_COLOR,
    BLACK_COLOR
  };

  explicit Board(int size = 7);

  int size() const { return size_; }

  // This method is used to verify if a given location is valid, i.e. it can be
  // used to place a piece on it or move a piece to it.
  bool IsValidLocation(const BoardLocation& location) const;

  // This function returns |true| if the two BoardLocations are adjacent. Both
  // locations must be valid.
  bool IsAdjacent(const BoardLocation& b1, const BoardLocation& b2) const;

  // Add the list of locations that are adjacent to |location| to the vector
  // that is passed as argument. It does not clear the contents of the vector.
  // |location| must be valid. The method does not check if the adjacent
  // locations are empty or not. It returns all of them.
  void GetAdjacentLocations(BoardLocation location,
      std::vector<BoardLocation>* adjacent_locations) const;

  // This method adds a new piece on the board. If the operation can be done,
  // it returns |true|; otherwise, if the location is already occupied or if
  // the location is not valid it returns |false|. Color cannot be NO_COLOR.
  bool AddPiece(const BoardLocation& location, PieceColor color);

  // Removes the piece at |location|. If the operation could be completed
  // successfully, the method returns |true|.
  bool RemovePiece(const BoardLocation& location);

  // Return the color of the piece at |location| or |PieceColor::NO_COLOR| if
  // there is no piece at that location. |location| must be valid.
  PieceColor GetPieceAt(const BoardLocation& location) const;

 private:
  // Utility method that returns the distance between valid locations on the
  // line or column specified by |index|.
  int GetStep(int index) const;

  int size_;

  std::map<BoardLocation, PieceColor> pieces_;

  DISALLOW_COPY_AND_ASSIGN(Board);
};

}  // namespace game

#endif  // GAME_BOARD_H_

