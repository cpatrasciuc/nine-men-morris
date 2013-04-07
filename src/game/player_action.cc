// Copyright (c) 2012 Cristian Patrasciuc. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "game/player_action.h"

#include "base/log.h"
#include "game/board.h"

namespace game {

PlayerAction::PlayerAction(PieceColor player_color, ActionType type)
    : player_color_(player_color),
      action_type_(type),
      source_(-1, -1),
      destination_(-1, -1) {
  DCHECK(player_color != NO_COLOR);
}

bool PlayerAction::CanExecuteOn(const Board& board) const {
  switch (action_type_) {
    case MOVE_PIECE:
      return board.IsValidLocation(source_) &&
             board.IsValidLocation(destination_) &&
             (board.GetPieceAt(source_) == player_color_) &&
             (board.GetPieceAt(destination_) == NO_COLOR);
    case PLACE_PIECE:
      return board.IsValidLocation(destination_) &&
             (board.GetPieceAt(destination_) == NO_COLOR);
    case REMOVE_PIECE:
      if (!board.IsValidLocation(source_)) {
        return false;
      }
      const PieceColor opponent = GetOpponent(player_color_);
      if (board.GetPieceAt(source_) != opponent) {
        return false;
      }
      // You can only remove a piece that is part of a mill, if there is no
      // piece available that is not part of a mill.
      if (board.IsPartOfMill(source_)) {
        for (int i = 0; i < board.size(); ++i) {
          for (int j = 0; j < board.size(); ++j) {
            const BoardLocation location(i, j);
            if (board.IsValidLocation(location)) {
              if (board.GetPieceAt(location) == opponent) {
                if (!board.IsPartOfMill(location)) {
                  return false;
                }
              }
            }
          }
        }
      }
      return true;
  }
  NOTREACHED();
  return false;
}

void PlayerAction::Execute(Board* board) const {
  DCHECK(CanExecuteOn(*board));
  switch (action_type_) {
    case MOVE_PIECE:
      board->MovePiece(source_, destination_);
      break;
    case PLACE_PIECE:
      board->AddPiece(destination_, player_color_);
      break;
    case REMOVE_PIECE:
      board->RemovePiece(source_);
      break;
  }
}

bool PlayerAction::CanUndoFrom(const Board& board) const {
  switch (action_type_) {
    case MOVE_PIECE:
      return board.IsValidLocation(source_) &&
             board.IsValidLocation(destination_) &&
             (board.GetPieceAt(destination_) == player_color_) &&
             (board.GetPieceAt(source_) == NO_COLOR);
    case PLACE_PIECE:
      return board.IsValidLocation(destination_) &&
             (board.GetPieceAt(destination_) == player_color_);
    case REMOVE_PIECE:
      return board.IsValidLocation(source_) &&
             (board.GetPieceAt(source_) == NO_COLOR);
  }
  NOTREACHED();
  return false;
}

void PlayerAction::Undo(Board* board) const {
  DCHECK(CanUndoFrom(*board));
  switch (action_type_) {
    case MOVE_PIECE:
      board->MovePiece(destination_, source_);
      break;
    case PLACE_PIECE:
      board->RemovePiece(destination_);
      break;
    case REMOVE_PIECE:
      board->AddPiece(source_, GetOpponent(player_color_));
      break;
  }
}

bool PlayerAction::IsJumpOn(const Board& board) const {
  if (action_type_ != MOVE_PIECE) {
    return false;
  }
  if (!board.IsValidLocation(source_) || !board.IsValidLocation(destination_)) {
    return false;
  }
  return !board.IsAdjacent(source_, destination_);
}

}  // namespace game
