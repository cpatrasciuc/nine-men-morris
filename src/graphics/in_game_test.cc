// Copyright (c) 2013 Cristian Patrasciuc. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "graphics/in_game_test.h"

#include "base/bind.h"
#include "base/basic_macros.h"
#include "base/callable.h"
#include "base/location.h"
#include "base/method.h"
#include "base/ptr/scoped_ptr.h"
#include "graphics/ogre_app.h"
#include "graphics/game_state.h"
#include "gtest/gtest.h"

#include "OGRE/OgreFrameListener.h"
#include "OGRE/OgreRoot.h"

namespace graphics {
namespace {

class RunTestMethodGameState : public GameState {
 public:
  explicit RunTestMethodGameState(InGameTestBase* in_game_test)
      : GameState(in_game_test->app()), in_game_test_(in_game_test) {}

  virtual bool frameRenderingQueued(const Ogre::FrameEvent& event) {
    static bool first_call = true;
    if (first_call) {
      in_game_test_->TestMethod();
      first_call = false;
    }
    return !in_game_test_->HasFatalFailure();
  }

 private:
  InGameTestBase* const in_game_test_;

  DISALLOW_COPY_AND_ASSIGN(RunTestMethodGameState);
};

}  // anonymous namespace

// TODO(in_game_tests): Remvome this once OgreApp is a singleton.
OgreApp* g_ogre_app = NULL;

InGameTestBase::InGameTestBase() : first_state_(NULL) {}

InGameTestBase::~InGameTestBase() {}

OgreApp* InGameTestBase::app() { return g_ogre_app; }

void InGameTestBase::SetUp() {
  // TODO(game_test): Provide an ogre config file for test.
  if (!g_ogre_app) {
    g_ogre_app = new OgreApp("Game Tests");
    ASSERT_TRUE(g_ogre_app->Init());
  }
  Reset(first_state_, new RunTestMethodGameState(this));
  app()->PushState(Get(first_state_));
  testing::UnitTest::GetInstance()->listeners().Append(this);
}

void InGameTestBase::TearDown() {
  testing::UnitTest::GetInstance()->listeners().Release(this);
}

void InGameTestBase::PostDoneTaskOnGameLoop() {
  static bool already_called = false;
  if (already_called) {
    return;
  }
  already_called = true;
  typedef void(InGameTestBase::*DoneSig)(void);
  base::Closure* done_task = base::Bind(
      new base::Method<DoneSig>(&InGameTestBase::Done), this);
  app()->PostTaskOnGameLoop(FROM_HERE, done_task);
}

void InGameTestBase::Done() {
  ASSERT_TRUE(Get(first_state_));
  ASSERT_EQ(Get(first_state_), app()->PopState());
  Reset(first_state_);
}

void InGameTestBase::OnTestPartResult(const testing::TestPartResult& result) {
  if (result.fatally_failed() || result.passed()) {
    PostDoneTaskOnGameLoop();
  }
}

}  // namespace graphics
