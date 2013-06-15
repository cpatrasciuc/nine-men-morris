// Copyright (c) 2013 Cristian Patrasciuc. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef GRAPHICS_PLAYING_STATE_H_
#define GRAPHICS_PLAYING_STATE_H_

#include <memory>

#include "base/ptr/scoped_ptr.h"
#include "game/game.h"
#include "game/game_options.h"
#include "graphics/board_view.h"
#include "graphics/camera_controller.h"
#include "graphics/game_state.h"
#include "graphics/graphics_export.h"
#include "graphics/player_delegate.h"

namespace game {
class PlayerAction;
}

namespace graphics {

class GRAPHICS_EXPORT PlayingState : public GameState {
 public:
  explicit PlayingState(OgreApp* app,
                        std::auto_ptr<game::Game> game_model);

  virtual bool Initialize();
  virtual void Exit();

 private:
  // KeyListener overrides
  virtual bool keyPressed(const OIS::KeyEvent& event);

  // MouseListener overrides
  virtual bool mouseMoved(const OIS::MouseEvent& event);
  virtual bool mousePressed(const OIS::MouseEvent& event,
                            OIS::MouseButtonID id);
  virtual bool mouseReleased(const OIS::MouseEvent& event,
                             OIS::MouseButtonID id);

  void InitializePlayers();
  void RequestPlayerAction();

  void ExecuteAction(const game::PlayerAction& action);

  base::ptr::scoped_ptr<game::Game> game_;
  base::ptr::scoped_ptr<BoardView> board_view_;
  CameraController camera_controller_;
  base::ptr::scoped_ptr<PlayerDelegate> white_player_;
  base::ptr::scoped_ptr<PlayerDelegate> black_player_;
};

}  // namespace graphics

#endif  // GRAPHICS_PLAYING_STATE_H_

