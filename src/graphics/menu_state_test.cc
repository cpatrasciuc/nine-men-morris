// Copyright (c) 2013 Cristian Patrasciuc. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include <memory>
#include <string>

#include "base/log.h"
#include "graphics/in_game_test.h"
#include "graphics/menu_state.h"
#include "graphics/ois_test_utils.h"
#include "gtest/gtest.h"

#include "OGRE/OgreOverlay.h"
#include "OGRE/OgreOverlayContainer.h"
#include "OGRE/OgreOverlayElement.h"
#include "OGRE/OgreOverlayManager.h"

#include "OIS/OISMouse.h"

namespace graphics {
namespace {

const char kTestMenuOverlayName[] = "TestMenu";

class MenuStateTest : public InGameTestBase {
 public:
  class TestMenuDelegate : public MenuState::Delegate {
   public:
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
    delegate_ = new TestMenuDelegate;
    menu_ = new MenuState(kTestMenuOverlayName,
                          std::auto_ptr<MenuState::Delegate>(delegate_));
    app()->PushState(menu_);
    return true;
  }

  virtual void TestMethod() {
    const char* const menu_buttons[] = {
        "MenuButton1", "MenuButton2", "MenuButton3" };
    const char* const menu_options[] = { "Option1", "Option2", "Option3" };

    EXPECT_EQ(arraysize(menu_buttons), arraysize(menu_options));
    for (size_t i = 0; i < arraysize(menu_buttons); ++i) {
      delegate_->clear_selection();
      SimulateClickOnOverlayElement(menu_buttons[i]);
      EXPECT_EQ(menu_options[i], delegate_->selected_option()) << i;
    }

    delegate_->clear_selection();
    SimulateClick(menu_, 0, 0);
    EXPECT_EQ("", delegate_->selected_option());

    // TODO(menu_state_test): Add test for keyboard events.
    SUCCEED();
  }

  virtual void Done() {
    EXPECT_EQ(menu_, app()->PopState());
    InGameTestBase::Done();
  }

 private:
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
    SimulateClick(menu_, x, y);
  }

  // TODO(menu_state_test): Make this a scoped_ptr.
  MenuState* menu_;
  TestMenuDelegate* delegate_;
};

IN_GAME_TEST(MenuStateTest, MenuStateTest);

}  // anonymous namespace
}  // namespace graphics
