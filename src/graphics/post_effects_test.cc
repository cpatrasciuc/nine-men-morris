// Copyright (c) 2013 Cristian Patrasciuc. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "graphics/in_game_test.h"
#include "graphics/post_effects.h"
#include "gtest/gtest.h"

#include "OGRE/OgreRenderWindow.h"

namespace graphics {
namespace {

class VignetteTest : public InGameTestBase {
 public:
  virtual void SetUp() {
    InGameTestBase::SetUp();
    SetVignettingEnabled(true);
  }

  virtual void TestMethod() {
    // Remove this and run test individually to manually check the effect.
    SUCCEED();
  }

  virtual void TearDown() {
    SetVignettingEnabled(false);
    InGameTestBase::TearDown();
  }
};

IN_GAME_TEST(VignetteTest, VignetteTest);

}  // anonymous namespace
}  // namespace graphics
