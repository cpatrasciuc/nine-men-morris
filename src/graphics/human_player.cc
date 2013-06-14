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
  view_->SetSelectionType(BoardView::EMPTY_LOCATION);
  Reset(action_,
      new game::PlayerAction(color(), game_model.next_action_type()));
}

void HumanPlayer::OnLocationSelected(const game::BoardLocation& location) {
  DCHECK(Get(action_));
  switch (action_->type()) {
    case game::PlayerAction::PLACE_PIECE:
      action_->set_destination(location);
      ExecuteAction();
      break;
    case game::PlayerAction::MOVE_PIECE:
    case game::PlayerAction::REMOVE_PIECE:
      NOTREACHED();
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
  Reset(action_);
  callback_.reset();
}

}  // namespace graphics
