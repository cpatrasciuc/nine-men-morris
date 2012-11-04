// Copyright (c) 2012 Cristian Patrasciuc. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef GAME_PLAYER_ACTION_H_
#define GAME_PLAYER_ACTION_H_

#include "game/board.h"
#include "game/board_location.h"
#include "game/game_export.h"

namespace game {

// Abstract class that represents an action that one player can take when it's
// his turn at the board. The can be on of the following:
//   - place a piece on the board;
//   - move a piece on the board:
//       - slide a piece between two adjacent locations;
//       - 'jump' a piece to any empty location if the player has less than
//         three pieces and GameOptions::jumps_allowed() is |true|;
//   - remove an opponent's piece after successfully closing a mill;
class GAME_EXPORT PlayerAction {
 public:
  enum ActionType {
    PLACE_PIECE,
    MOVE_PIECE,
    REMOVE_PIECE
  };

  explicit PlayerAction(Board::PieceColor player_color, ActionType type);

  Board::PieceColor player_color() const { return player_color_; }

  ActionType type() const { return action_type_; }

  const BoardLocation& source() const { return source_; }
  void set_source(const BoardLocation& source) { source_ = source; }

  const BoardLocation& destination() const { return destination_; }
  void set_destination(const BoardLocation& destination) {
    destination_ = destination;
  }

  // Execute the action on the given board instance.
  void Execute(Board* board) const;

  // Undo the action that was previously executed on the given board instance.
  void Undo(Board* board) const;

 private:
  Board::PieceColor player_color_;
  ActionType action_type_;
  BoardLocation source_;
  BoardLocation destination_;
};

}  // namespace game

#endif  // GAME_PLAYER_ACTION_H_

