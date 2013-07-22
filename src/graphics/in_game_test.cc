// Copyright (c) 2013 Cristian Patrasciuc. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "graphics/in_game_test.h"

#include "base/bind.h"
#include "base/basic_macros.h"
#include "base/callable.h"
#include "base/location.h"
#include "base/method.h"
#include "graphics/ogre_app.h"
#include "graphics/game_state.h"
#include "gtest/gtest.h"

#include "OGRE/OgreFrameListener.h"
#include "OGRE/OgreRoot.h"

namespace graphics {

InGameTestBase::InGameTestBase() {}

InGameTestBase::~InGameTestBase() {}

void InGameTestBase::SetUp() {
  // TODO(game_test): Provide an ogre config file for test.
  ASSERT_TRUE(OgreApp::Instance().Init());
  app()->PushState(this);
  testing::UnitTest::GetInstance()->listeners().Append(this);
}

void InGameTestBase::TearDown() {
  app()->ShutDown();
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
  ASSERT_EQ(this, app()->PopState());
}

bool InGameTestBase::frameRenderingQueued(const Ogre::FrameEvent& event) {
  static bool first_call = true;
  if (first_call) {
    TestMethod();
    first_call = false;
  }
  return !HasFatalFailure();
}

void InGameTestBase::OnTestPartResult(const testing::TestPartResult& result) {
  if (result.fatally_failed() || result.passed()) {
    PostDoneTaskOnGameLoop();
  }
}

}  // namespace graphics
