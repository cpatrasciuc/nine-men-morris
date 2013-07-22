// Copyright (c) 2013 Cristian Patrasciuc. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "base/log.h"
#include "base/ptr/scoped_ptr.h"
#include "graphics/game_state.h"
#include "graphics/in_game_test.h"
#include "graphics/ogre_app.h"
#include "graphics/ois_test_utils.h"
#include "gtest/gtest.h"

#include "OIS/OISKeyboard.h"
#include "OIS/OISMouse.h"

namespace graphics {
namespace {

void SimulateIOEvents() {
  OgreApp& app = OgreApp::Instance();
  SimulateClick(&app, 0, 0);
  SimulateKeyPress(&app, OIS::KC_0);
}

class GameStateTest : public InGameTestBase {
 public:
  virtual void TestMethod() {
    Reset(event_counting_state_, new EventCountingState);
    OgreApp::Instance().PushState(Get(event_counting_state_));
  }

  virtual void Done() {
    EXPECT_EQ(Get(event_counting_state_), OgreApp::Instance().PopState());
    Reset(event_counting_state_);
    InGameTestBase::Done();
  }

 private:
  class DummyState : public GameState {
   public:
    virtual bool frameRenderingQueued(const Ogre::FrameEvent&) {
     static int calls = 0;
     ++calls;
     SimulateIOEvents();
     if (calls == 5) {
      EXPECT_EQ(this, OgreApp::Instance().PopState());
     }
     return true;
    }
  };

  class EventCountingState : public GameState {
   public:
    EventCountingState()
        : init_(0),
          exit_(0),
          pause_(0),
          resume_(0),
          frame_started_(0),
          frame_(0),
          frame_ended_(0),
          key_pressed_(0),
          key_released_(0),
          mouse_moved_(0),
          mouse_pressed_(0),
          mouse_released_(0) {}

    // GameState overrides.
    virtual bool Initialize() {
      ++init_;
      return true;
    }
    virtual void Exit() { EXPECT_EQ(1, ++exit_); }
    virtual void Pause() { ++pause_; }
    virtual void Resume() { ++resume_; }

    // FrameListener interface
    virtual bool frameStarted(const Ogre::FrameEvent&) {
      ++frame_started_;
      return true;
    }

    virtual bool frameEnded(const Ogre::FrameEvent& event) {
      ++frame_ended_;
      return true;
    }

    virtual bool frameRenderingQueued(const Ogre::FrameEvent&) {
      if (frame_ == 0) {
        EXPECT_EQ(1, init_);
        EXPECT_EQ(0, exit_);
        EXPECT_EQ(0, pause_);
        EXPECT_EQ(0, resume_);
        EXPECT_EQ(1, frame_started_);
        EXPECT_EQ(0, frame_ended_);
        ExpectEventCount(0);
        SimulateIOEvents();
        ExpectEventCount(1);
        OgreApp::Instance().PushState(new DummyState);
      } else if (frame_ == 1) {
        EXPECT_EQ(1, init_);
        EXPECT_EQ(0, exit_);
        EXPECT_EQ(1, pause_);
        EXPECT_EQ(1, resume_);
        EXPECT_EQ(2, frame_started_);
        EXPECT_EQ(1, frame_ended_);
        ExpectEventCount(1);
        SUCCEED();
      } else {
        NOTREACHED();
      }
      ++frame_;
      return true;
    }

    // KeyListener interface
    virtual bool keyPressed(const OIS::KeyEvent&) {
      ++key_pressed_;
      return true;
    }
    virtual bool keyReleased(const OIS::KeyEvent&) {
      ++key_released_;
      return true;
    }

    // MouseListener interface
    virtual bool mouseMoved(const OIS::MouseEvent&) {
      ++mouse_moved_;
      return true;
    }
    virtual bool mousePressed(const OIS::MouseEvent&, OIS::MouseButtonID) {
      ++mouse_pressed_;
      return true;
    }
    virtual bool mouseReleased(const OIS::MouseEvent&, OIS::MouseButtonID) {
      ++mouse_released_;
      return true;
    }

    void ExpectEventCount(int expected_count) {
      EXPECT_EQ(expected_count, key_pressed_);
      EXPECT_EQ(expected_count, key_released_);
      EXPECT_EQ(expected_count, mouse_moved_);
      EXPECT_EQ(expected_count, mouse_pressed_);
      EXPECT_EQ(expected_count, mouse_released_);
    }

    int init_;
    int exit_;
    int pause_;
    int resume_;
    int frame_started_;
    int frame_;
    int frame_ended_;
    int key_pressed_;
    int key_released_;
    int mouse_moved_;
    int mouse_pressed_;
    int mouse_released_;

    DISALLOW_COPY_AND_ASSIGN(EventCountingState);
  };

  base::ptr::scoped_ptr<EventCountingState> event_counting_state_;
};

IN_GAME_TEST(GameStateTest, GameStateTest);

}  // anonymous namespace
}  // namespace graphics
