// Copyright (c) 2013 Cristian Patrasciuc. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include <string>

#include "base/ptr/scoped_ptr.h"
#include "graphics/confirmation_menu_state.h"
#include "graphics/in_game_test.h"
#include "gtest/gtest.h"

#include "OGRE/OgreFrameListener.h"
#include "OGRE/OgreOverlayElement.h"
#include "OGRE/OgreOverlayManager.h"

namespace graphics {
namespace {

class ConfirmationMenuStateWrapper : public ConfirmationMenuState {
 public:
  ConfirmationMenuStateWrapper(const std::string& question,
                               const std::string& option)
      : ConfirmationMenuState(question),
        question_(question),
        option_(option),
        should_be_visible_(true) {}

  virtual bool frameRenderingQueued(const Ogre::FrameEvent& event) {
    static bool was_called = false;
    EXPECT_TRUE(should_be_visible_);
    if (!was_called) {
      OnMenuOptionSelected("invalid option");
    } else {
      Ogre::OverlayManager& overlay_mgr = Ogre::OverlayManager::getSingleton();
      Ogre::OverlayElement* const question_text_area =
          overlay_mgr.getOverlayElement("QuestionText");
      EXPECT_EQ(question_, std::string(question_text_area->getCaption()));
      OnMenuOptionSelected(option_);
      should_be_visible_ = false;
    }
    was_called = true;
    return true;
  }

 private:
  const std::string question_;
  const std::string option_;
  bool should_be_visible_;
};

class ConfirmationMenuStateTest : public InGameTestBase {
 public:
  virtual void TestMethod() {
    index_ = 0;
    RunNextTest();
  }

  virtual void Resume() {
    EXPECT_EQ(expected_result_, menu_->is_confirmed());
    ++index_;
    RunNextTest();
  }

 private:
  void RunNextTest() {
    static const char* const questions[] = { "Test Yes", "Test No" };
    static const char* const options[] = { "Yes", "No" };
    static const bool expected_results[] = { true, false };

    EXPECT_EQ(arraysize(questions), arraysize(options));
    EXPECT_EQ(arraysize(questions), arraysize(expected_results));

    if (index_ == arraysize(questions)) {
      SUCCEED();
    } else {
      expected_result_ = expected_results[index_];
      Reset(menu_,
          new ConfirmationMenuStateWrapper(questions[index_], options[index_]));
      app()->PushState(Get(menu_));
    }
  }

  int index_;
  bool expected_result_;
  base::ptr::scoped_ptr<ConfirmationMenuState> menu_;
};

IN_GAME_TEST(ConfirmationMenuStateTest, ConfirmationMenuStateTest);

}  // anonymous namespace
}  // namespace graphics
