// Copyright (c) 2012 Cristian Patrasciuc. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "game/player_action.h"

#include "base/log.h"

namespace game {

PlayerAction::PlayerAction(Board::PieceColor player_color, ActionType type)
    : player_color_(player_color),
      action_type_(type),
      source_(-1, -1),
      destination_(-1, -1) {
  DCHECK(player_color != Board::NO_COLOR);
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

void PlayerAction::Undo(Board* board) const {
  switch (action_type_) {
    case MOVE_PIECE:
      DCHECK(board->IsValidLocation(source_));
      DCHECK(board->IsValidLocation(destination_));
      DCHECK_EQ(player_color_, board->GetPieceAt(destination_));
      DCHECK_EQ(Board::NO_COLOR, board->GetPieceAt(source_));
      board->MovePiece(destination_, source_);
      break;
    case PLACE_PIECE:
      DCHECK(board->IsValidLocation(destination_));
      DCHECK_EQ(player_color_, board->GetPieceAt(destination_));
      board->RemovePiece(destination_);
      break;
    case REMOVE_PIECE:
      DCHECK(board->IsValidLocation(source_));
      DCHECK_EQ(Board::NO_COLOR, board->GetPieceAt(source_));
      board->AddPiece(source_, player_color_ == Board::WHITE_COLOR ?
        Board::BLACK_COLOR : Board::WHITE_COLOR);
      break;
  }
}

bool PlayerAction::CanExecuteOn(const Board& board) const {
  switch (action_type_) {
    case MOVE_PIECE:
      return board.IsValidLocation(source_) &&
             board.IsValidLocation(destination_) &&
             (board.GetPieceAt(source_) == player_color_) &&
             (board.GetPieceAt(destination_) == Board::NO_COLOR);
    case PLACE_PIECE:
      return board.IsValidLocation(destination_) &&
             (board.GetPieceAt(destination_) == Board::NO_COLOR);
    case REMOVE_PIECE:
      return board.IsValidLocation(source_) &&
             (board.GetPieceAt(source_) ==
                  (player_color_ == Board::WHITE_COLOR ?
                   Board::BLACK_COLOR : Board::WHITE_COLOR));
  }
  NOTREACHED();
  return false;
}

bool PlayerAction::CanUndoFrom(const Board& board) const {
  switch (action_type_) {
    case MOVE_PIECE:
      return board.IsValidLocation(source_) &&
             board.IsValidLocation(destination_) &&
             (board.GetPieceAt(destination_) == player_color_) &&
             (board.GetPieceAt(source_) == Board::NO_COLOR);
    case PLACE_PIECE:
      return board.IsValidLocation(destination_) &&
             (board.GetPieceAt(destination_) == player_color_);
    case REMOVE_PIECE:
      return board.IsValidLocation(source_) &&
             (board.GetPieceAt(source_) == Board::NO_COLOR);
  }
  NOTREACHED();
  return false;
}

}  // namespace game
