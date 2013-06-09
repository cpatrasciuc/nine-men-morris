// Copyright (c) 2012 Cristian Patrasciuc. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef GAME_PLAYER_ACTION_H_
#define GAME_PLAYER_ACTION_H_

#include "game/board_location.h"
#include "game/game_export.h"
#include "game/piece_color.h"

namespace game {

class Board;

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

  explicit PlayerAction(PieceColor player_color, ActionType type);

  PieceColor player_color() const { return player_color_; }

  ActionType type() const { return action_type_; }

  const BoardLocation& source() const { return source_; }
  void set_source(const BoardLocation& source) { source_ = source; }

  const BoardLocation& destination() const { return destination_; }
  void set_destination(const BoardLocation& destination) {
    destination_ = destination;
  }

  // Verifies if |this| action satisfies all the constraints required before
  // executing it on the given |board|. If all pre-conditions are verified the
  // method returns |true|.
  //
  // Note: For MOVE_PIECE actions it does not verify if the locations are
  //       adjacent. This allows jumps at the end of the game.
  bool CanExecuteOn(const Board& board) const;

  // Execute the action on the given board instance. The action must be a valid
  // one given the current state of the board. To check this you can run
  // |CanExecuteOn()|.
  void Execute(Board* board) const;

  // Verifies if |this| action satisfies all the constraints required before
  // undoing it from the given |board|. If all the pre-conditions are verified
  // the method returns |true|.
  // See also the note from |CanExecuteOn()| regarding MOVE_PIECE actions.
  bool CanUndoFrom(const Board& board) const;

  // Undo the action that was previously executed on the given board instance.
  void Undo(Board* board) const;

  // The method tells if this player action is a jump action. A jump is a
  // special type of MOVE_PIECE action, when the source and destination are not
  // adjacent, given the |board|. It is allowed during the end phase of the game
  // when the player has less than four pieces left on the board. The method
  // does not verify if the move can actually be executed on the board. For that
  // please use |CanExecuteOn()|.
  // Also see |GameOptions::jumps_allowed()|.
  bool IsJumpOn(const Board& board) const;

 private:
  PieceColor player_color_;
  ActionType action_type_;
  BoardLocation source_;
  BoardLocation destination_;
};

}  // namespace game

#endif  // GAME_PLAYER_ACTION_H_

