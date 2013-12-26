// Copyright (c) 2013 Cristian Patrasciuc. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef GRAPHICS_PLAYING_STATE_H_
#define GRAPHICS_PLAYING_STATE_H_

#include <memory>

#include "base/ptr/scoped_ptr.h"
#include "game/game.h"
#include "game/game_options.h"
#include "game/mill_events_generator.h"
#include "graphics/board_view.h"
#include "graphics/camera_controller.h"
#include "graphics/confirmation_menu_state.h"
#include "graphics/game_state.h"
#include "graphics/graphics_export.h"
#include "graphics/player_delegate.h"

namespace game {
class PlayerAction;
}

namespace Ogre {
struct FrameEvent;
}

namespace graphics {

class GRAPHICS_EXPORT PlayingState : public GameState {
 public:
  PlayingState(std::auto_ptr<game::Game> game_model,
               std::auto_ptr<PlayerDelegate> white_player,
               std::auto_ptr<PlayerDelegate> black_player);

  BoardView* board_view() { return Get(board_view_); }

  const game::Game& game_model() const { return *game_; }

  virtual bool Initialize();
  virtual void Exit();
  virtual void Resume();

 private:
  // KeyListener overrides
  virtual bool keyReleased(const OIS::KeyEvent& event);

  // MouseListener overrides
  virtual bool mouseMoved(const OIS::MouseEvent& event);
  virtual bool mousePressed(const OIS::MouseEvent& event,
                            OIS::MouseButtonID id);
  virtual bool mouseReleased(const OIS::MouseEvent& event,
                             OIS::MouseButtonID id);

  // FrameListener overrides
  virtual bool frameRenderingQueued(const Ogre::FrameEvent& event);

  void InitializePlayers();
  void RequestPlayerAction();

  void ExecuteAction(const game::PlayerAction& action);

  base::ptr::scoped_ptr<game::Game> game_;
  base::ptr::scoped_ptr<BoardView> board_view_;
  base::ptr::scoped_ptr<game::MillEventsGenerator> mill_events_generator_;
  CameraController camera_controller_;
  base::ptr::scoped_ptr<PlayerDelegate> white_player_;
  base::ptr::scoped_ptr<PlayerDelegate> black_player_;
  bool stopped_;
  base::ptr::scoped_ptr<ConfirmationMenuState> pause_menu_;
};

}  // namespace graphics

#endif  // GRAPHICS_PLAYING_STATE_H_

