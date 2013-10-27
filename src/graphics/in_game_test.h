// Copyright (c) 2013 Cristian Patrasciuc. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef GRAPHICS_IN_GAME_TEST_H_
#define GRAPHICS_IN_GAME_TEST_H_

#include "base/basic_macros.h"
#include "graphics/game_state.h"
#include "graphics/ogre_app.h"
#include "graphics/out_of_process_test.h"
#include "gtest/gtest.h"

namespace graphics {

class InGameTestBase
    : public GameState,
      public testing::Test,
      public testing::EmptyTestEventListener {
 public:
  // Must be overwritten by specific test cases. This method will be called on
  // the game loop after the first frame is queued for rendering.
  virtual void TestMethod() = 0;

 protected:
  InGameTestBase();
  virtual ~InGameTestBase();

  // Initializes |app_| and pushes |this| on the game states stack. It also
  // schedules TestMethod() to be called on the main game loop.
  virtual void SetUp();

  virtual void TearDown();

  // Must be called when the test is over and the game loop can exit. It calls
  // |PostTaskOnGameLoop| and passed a call to |Done()| as the task.
  void PostDoneTaskOnGameLoop();

  // It checks that |this| is the only game state left on the stack and
  // pops it. If it is overridden in subclasses, the overridden method must call
  // InGameTestBase::Done() at the end.
  virtual void Done();

 private:
  // Called during SetUp() to post a call to TestMethod() on the game loop. This
  // ensures that TestMethod() will be called even if during SetUp() a new game
  // state is pushed on the stack on top of |this|.
  void PostTestTaskOnGameLoop();

  // testing::EmptyTestEventListener overrides
  // Listener for partial results. If a fatal failure or SUCCEED() occurred, it
  // calls PostDoneTaskOnGameLoop() to end the test.
  virtual void OnTestPartResult(const testing::TestPartResult& result);

  DISALLOW_COPY_AND_ASSIGN(InGameTestBase);
};

#define IN_GAME_TEST(test_class, test_name) \
  TEST_F(test_class, test_name) { \
    app()->RunMainLoop(); \
  } \
  OOP_TEST_F(test_class, test_name);

}  // namespace graphics

#endif  // GRAPHICS_IN_GAME_TEST_H_

