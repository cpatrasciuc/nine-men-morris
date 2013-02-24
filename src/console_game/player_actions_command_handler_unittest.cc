// Copyright (c) 2013 Cristian Patrasciuc. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include <memory>

#include "base/basic_macros.h"
#include "console_game/command_handler.h"
#include "console_game/player_actions_command_handler.h"
#include "game/game.h"
#include "game/game_test_helper.h"
#include "gtest/gtest.h"

namespace console_game {
namespace {

TEST(PlayerActionsCommandHelper, FullCommands) {
  const char* const command_types[] = { "PLACE", "moVe", "remove" };
  const char* const arguments[] = { "b1", "b3 A3", "1C" };
  std::auto_ptr<game::Game> test_game =
      game::LoadSavedGameForTests("actions_test_3");
  ASSERT_TRUE(test_game.get());
  PlayerActionsCommandHandler actions_handler;
  CommandHandler* handler(&actions_handler);
  ASSERT_EQ(arraysize(command_types), arraysize(arguments));
  for (size_t i = 0; i < arraysize(command_types); ++i) {
    ASSERT_TRUE(handler->ProcessCommand(
        command_types[i], arguments[i], test_game.get()));
  }
  for (size_t i = 0; i < arraysize(command_types); ++i) {
    ASSERT_FALSE(handler->ProcessCommand(
        command_types[i], arguments[i], test_game.get()));
  }
}

TEST(PlayerActionsCommandHelper, SimplifiedCommands) {
  const char* const command_types[] = { "", "", "" };
  const char* const arguments[] = { "b1", "b3 A3", "1C" };
  std::auto_ptr<game::Game> test_game =
      game::LoadSavedGameForTests("actions_test_3");
  ASSERT_TRUE(test_game.get());
  PlayerActionsCommandHandler actions_handler;
  CommandHandler* handler(&actions_handler);
  ASSERT_EQ(arraysize(command_types), arraysize(arguments));
  for (size_t i = 0; i < arraysize(command_types); ++i) {
    ASSERT_TRUE(handler->ProcessCommand(
        command_types[i], arguments[i], test_game.get()));
  }
  for (size_t i = 0; i < arraysize(command_types); ++i) {
    ASSERT_FALSE(handler->ProcessCommand(
        command_types[i], arguments[i], test_game.get()));
  }
}

TEST(PlayerActionsCommandHelper, InvalidCommands) {
  std::auto_ptr<game::Game> test_game =
      game::LoadSavedGameForTests("actions_test_3");
  ASSERT_TRUE(test_game.get());
  PlayerActionsCommandHandler actions_handler;
  CommandHandler* handler(&actions_handler);
  // Empty command
  ASSERT_FALSE(handler->ProcessCommand("", "", test_game.get()));
  // Invalid command type
  ASSERT_FALSE(handler->ProcessCommand("invalid_type", "", test_game.get()));
  // Invalid location (already occupied)
  ASSERT_FALSE(handler->ProcessCommand("", "a1", test_game.get()));
  // Invalid location (outside of the board)
  ASSERT_FALSE(handler->ProcessCommand("place", "A0", test_game.get()));
  // Invalid action type
  ASSERT_FALSE(handler->ProcessCommand("remove", "b1", test_game.get()));
}

TEST(PlayerActionsCommandHelperDeathTest, DEBUG_ONLY_TEST(NullGame)) {
  ::testing::FLAGS_gtest_death_test_style = "threadsafe";
  PlayerActionsCommandHandler actions_handler;
  CommandHandler* handler(&actions_handler);
  ASSERT_DEATH(handler->ProcessCommand("type", "args", NULL), "");
}

}  // anonymous namespace
}  // namespace console_game
