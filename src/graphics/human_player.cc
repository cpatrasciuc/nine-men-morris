// Copyright (c) 2013 Cristian Patrasciuc. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "graphics/human_player.h"

#include <memory>

#include "base/log.h"
#include "base/ptr/scoped_ptr.h"
#include "game/game.h"
#include "game/piece_color.h"
#include "game/player_action.h"
#include "graphics/board_view.h"
#include "graphics/player_delegate.h"
#include "graphics/selection_listener.h"

namespace graphics {

HumanPlayer::HumanPlayer(game::PieceColor color)
    : PlayerDelegate(color), view_(NULL), action_(NULL), callback_(NULL) {}

void HumanPlayer::RequestAction(const game::Game& game_model,
                                std::auto_ptr<PlayerActionCallback> callback) {
  DCHECK(view_);
  callback_ = callback;
  view_->AddListener(this);
  // TODO(human_player): Temporary implementation for test.
  Reset(action_,
      new game::PlayerAction(color(), game_model.next_action_type()));
  switch (action_->type()) {
    case game::PlayerAction::PLACE_PIECE:
      view_->SetSelectionType(BoardView::EMPTY_LOCATION);
      break;
    case game::PlayerAction::REMOVE_PIECE:
      if (color() == game::WHITE_COLOR) {
        view_->SetSelectionType(BoardView::REMOVABLE_BLACK_PIECE);
      } else {
        view_->SetSelectionType(BoardView::REMOVABLE_WHITE_PIECE);
      }
      break;
    case game::PlayerAction::MOVE_PIECE:
      if (color() == game::WHITE_COLOR) {
        view_->SetSelectionType(BoardView::ANY_WHITE_PIECE);
      } else {
        view_->SetSelectionType(BoardView::ANY_BLACK_PIECE);
      }
      break;
  }
}

void HumanPlayer::OnLocationSelected(const game::BoardLocation& location) {
  DCHECK(Get(action_));
  switch (action_->type()) {
    case game::PlayerAction::PLACE_PIECE:
      action_->set_destination(location);
      ExecuteAction();
      break;
    case game::PlayerAction::REMOVE_PIECE:
      action_->set_source(location);
      ExecuteAction();
      break;
    case game::PlayerAction::MOVE_PIECE:
      if (view_->game_model().board().GetPieceAt(location) == color()) {
        action_->set_source(location);
        const BoardView::SelectionType own_pieces =
            color() == game::WHITE_COLOR ?
            BoardView::ANY_WHITE_PIECE :
            BoardView::ANY_BLACK_PIECE;
        view_->SetSelectionType(
            BoardView::EMPTY_LOCATION | own_pieces);
      } else {
        action_->set_destination(location);
        if (view_->game_model().CanExecutePlayerAction(*Get(action_))) {
          ExecuteAction();
        }
      }
      break;
  }
}

void HumanPlayer::OnSelectionCleared() {}

void HumanPlayer::ExecuteAction() {
  DCHECK(callback_.get());
  DCHECK(Get(action_));
  view_->RemoveListener(this);
  view_->SetSelectionType(BoardView::NONE);
  (*callback_)(*Get(action_));
}

}  // namespace graphics
