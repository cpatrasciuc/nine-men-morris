// Copyright (c) 2013 Cristian Patrasciuc. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include <string>

#include "base/log.h"
#include "base/ptr/scoped_ptr.h"
#include "graphics/in_game_test.h"
#include "graphics/menu_state.h"
#include "graphics/ois_test_utils.h"
#include "gtest/gtest.h"

#include "OGRE/OgreOverlay.h"
#include "OGRE/OgreOverlayContainer.h"
#include "OGRE/OgreOverlayElement.h"
#include "OGRE/OgreOverlayManager.h"

namespace graphics {
namespace {

const char kTestMenuOverlayName[] = "TestMenu";
const char* const kMenuButtons[] = {
    "MenuButton1", "MenuButton2", "MenuButton3" };
const char* const kMenuOptions[] = { "Option1", "Option2", "Option3" };

class MenuStateTest : public InGameTestBase {
 public:
  class TestMenu : public MenuState {
   public:
    TestMenu() : MenuState(kTestMenuOverlayName) {}

    virtual void OnMenuOptionSelected(const std::string& option) {
      LOG(INFO) << option;
      option_ = option;
    }

    const std::string& selected_option() const { return option_; }
    void clear_selection() { option_.clear(); }

   private:
    std::string option_;
  };

  virtual bool Initialize() {
    Reset(menu_, new TestMenu);
    app()->PushState(Get(menu_));
    return true;
  }

  virtual void TestMethod() {
    EXPECT_EQ(arraysize(kMenuButtons), arraysize(kMenuOptions));
    TestMouseEventsHandling();
    TestKeyboardEventsHandling();
    SUCCEED();
  }

  virtual void Done() {
    EXPECT_EQ(Get(menu_), app()->PopState());
    Reset(menu_);
    InGameTestBase::Done();
  }

 private:
  void TestMouseEventsHandling() {
    for (size_t i = 0; i < arraysize(kMenuButtons); ++i) {
      menu_->clear_selection();
      SimulateClickOnOverlayElement(kMenuButtons[i]);
      EXPECT_EQ(kMenuOptions[i], menu_->selected_option()) << i;
    }
    menu_->clear_selection();
    SimulateClick(Get(menu_), 0, 0);
    EXPECT_TRUE(menu_->selected_option().empty());
  }

  void TestKeyboardEventsHandling() {
    const char* const expected_options[] = { "", kMenuOptions[0], "" };
    for (size_t i = 0; i < arraysize(expected_options); ++i) {
      menu_->clear_selection();
      menu_->set_escape_option(expected_options[i]);
      SimulateKeyPress(Get(menu_), OIS::KC_RETURN);
      EXPECT_TRUE(menu_->selected_option().empty());
      SimulateKeyPress(Get(menu_), OIS::KC_ESCAPE);
      EXPECT_EQ(expected_options[i], menu_->selected_option());
    }
  }

  void SimulateClickOnOverlayElement(const std::string& element_name) {
    Ogre::Overlay* const menu_overlay =
        Ogre::OverlayManager::getSingleton().getByName(kTestMenuOverlayName);
    Ogre::OverlayContainer* const back_panel =
        menu_overlay->getChild("BackPanel");
    Ogre::OverlayElement* const button = back_panel->getChild(element_name);
    const double x =
        back_panel->getLeft() + button->getLeft() + button->getWidth() / 2.0;
    const double y =
        back_panel->getTop() + button->getTop() + button->getHeight() / 2.0;
    EXPECT_TRUE(button->contains(x, y))
        << button->getName() << ": " << x << " " << y;
    EXPECT_TRUE(menu_overlay->findElementAt(x, y));
    SimulateClick(Get(menu_), x, y);
  }

  base::ptr::scoped_ptr<TestMenu> menu_;
};

IN_GAME_TEST(MenuStateTest, MenuStateTest);

}  // anonymous namespace
}  // namespace graphics
