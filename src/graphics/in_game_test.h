// Copyright (c) 2013 Cristian Patrasciuc. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef GRAPHICS_IN_GAME_TEST_H_
#define GRAPHICS_IN_GAME_TEST_H_

#include "base/basic_macros.h"
#include "base/ptr/scoped_ptr.h"
#include "graphics/game_state.h"
#include "graphics/ogre_app.h"
#include "gtest/gtest.h"

namespace graphics {

class InGameTestBase : public ::testing::Test {
 public:
  // Must be overwritten by specific test cases. This method will be called on
  // the game loop after the first frame is queued for rendering.
  virtual void TestMethod() = 0;

  OgreApp* app() { return &app_; }

 protected:
  InGameTestBase();
  virtual ~InGameTestBase();

  // Initializes |app_| and pushes |this| on the game states stack.
  virtual void SetUp();

  // Must be called when the test is over and the game loop can exit. It checks
  // that |this| is the only game state left on the stack and pops it.
  void Done();

 private:
  OgreApp app_;

  // The first game state that is pushed on the stack when the test is run. A
  // pointer to it is stored to ensure that when the Done() method is called and
  // the test is over it is the only state remaining on the stack.
  base::ptr::scoped_ptr<GameState> first_state_;

  DISALLOW_COPY_AND_ASSIGN(InGameTestBase);
};

#define IN_GAME_TEST(test_class, test_name) \
  TEST_F(test_class, test_name) { \
    ASSERT_TRUE(app()); \
    app()->RunMainLoop(); \
  }

}  // namespace graphics

#endif  // GRAPHICS_IN_GAME_TEST_H_

