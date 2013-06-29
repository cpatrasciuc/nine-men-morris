// Copyright (c) 2013 Cristian Patrasciuc. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef GRAPHICS_IN_GAME_TEST_H_
#define GRAPHICS_IN_GAME_TEST_H_

#include "base/basic_macros.h"
#include "base/callable.h"
#include "base/ptr/scoped_ptr.h"
#include "graphics/game_state.h"
#include "graphics/ogre_app.h"
#include "gtest/gtest.h"

#include "OGRE/OgreWorkQueue.h"

namespace graphics {

class InGameTestBase
    : public ::testing::Test,
      public Ogre::WorkQueue::RequestHandler,
      public Ogre::WorkQueue::ResponseHandler {
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

  virtual void TearDown();

  // Must be called when the test is over and the game loop can exit. It checks
  // that |this| is the only game state left on the stack and pops it.
  void Done();

  // TODO(ogre_app): Move the "trampoline" feature to the OgreApp class.
  void PostTaskOnGameLoop(base::Closure* task);

 private:
  // Ogre::RequestHandler interface
  virtual Ogre::WorkQueue::Response* handleRequest(
      const Ogre::WorkQueue::Request* request,
      const Ogre::WorkQueue* source_queue);

  // Ogre::ResponseHandler interface
  virtual void handleResponse(const Ogre::WorkQueue::Response* response,
                              const Ogre::WorkQueue* source_queue);

  OgreApp app_;

  // The first game state that is pushed on the stack when the test is run. A
  // pointer to it is stored to ensure that when the Done() method is called and
  // the test is over it is the only state remaining on the stack.
  base::ptr::scoped_ptr<GameState> first_state_;

  // Ogre work queue channel used to bounce back tasks on the UI thread.
  Ogre::uint16 channel_;

  DISALLOW_COPY_AND_ASSIGN(InGameTestBase);
};

#define IN_GAME_TEST(test_class, test_name) \
  TEST_F(test_class, test_name) { \
    ASSERT_TRUE(app()); \
    app()->RunMainLoop(); \
  }

}  // namespace graphics

#endif  // GRAPHICS_IN_GAME_TEST_H_

