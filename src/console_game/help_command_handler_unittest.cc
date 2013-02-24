// Copyright (c) 2013 Cristian Patrasciuc. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include <map>
#include <sstream>
#include <string>
#include <utility>
#include <vector>

#include "base/basic_macros.h"
#include "console_game/command_handler.h"
#include "console_game/help_command_handler.h"
#include "gtest/gtest.h"

namespace console_game {
namespace {

const char kFormatSuffix[] = "format";
const char kUsageSuffix[] = "usage";

class DummyHandler : public CommandHandler {
 public:
  DummyHandler() {}

 private:
  // CommandHandler interface
  virtual std::vector<std::string> SupportedCommandTypes() const {
    return std::vector<std::string>();
  }

  virtual bool ProcessCommand(const std::string& command_type,
                              const std::string& args,
                              game::Game* game_model) {
    return true;
  }

  virtual void GetHelpMessage(const std::string& command_type,
                              std::string* format,
                              std::string* usage) {
    *format = command_type + kFormatSuffix;
    *usage = command_type + kUsageSuffix;
  };

  DISALLOW_COPY_AND_ASSIGN(DummyHandler);
};

TEST(HelpCommandHandler, Basic) {
  const char* const commands[] = { "foo", "bar", "baz" };
  std::map<std::string, CommandHandler*> handlers;
  std::ostringstream out;
  HelpCommandHandler handler(handlers, &out);

  for (size_t i = 0; i < arraysize(commands); ++i) {
    handlers.insert(std::make_pair(commands[i], new DummyHandler()));
  }

  static_cast<CommandHandler*>(&handler)->ProcessCommand("help", "", NULL);

  const std::string help_message(out.str());
  for (size_t i = 0; i < arraysize(commands); ++i) {
    const std::string expected_format(std::string(commands[i]) + kFormatSuffix);
    const std::string expected_usage(std::string(commands[i]) + kUsageSuffix);
    EXPECT_NE(std::string::npos, help_message.find(expected_format));
    EXPECT_NE(std::string::npos, help_message.find(expected_usage));
  }

  // TODO(stl): Replace this with scoped_values_map
  std::map<std::string, CommandHandler*>::iterator it;
  for (it = handlers.begin(); it != handlers.end(); ++it) {
    delete it->second;
  }
}

TEST(HelpCommandHandler, EmptyHandlers) {
  std::map<std::string, CommandHandler*> empty_map;
  std::ostringstream out;
  HelpCommandHandler handler(empty_map, &out);
  EXPECT_TRUE(
      static_cast<CommandHandler*>(&handler)->ProcessCommand("help", "", NULL));
  EXPECT_EQ("", out.str());
}

TEST(HelpCommandHelperDeathTest, DEBUG_ONLY_TEST(NullStream)) {
  ::testing::FLAGS_gtest_death_test_style = "threadsafe";
  std::map<std::string, CommandHandler*> empty_map;
  ASSERT_DEATH(HelpCommandHandler handler(empty_map, NULL), "");
}

}  // anonymous namespace
}  // namespace console_game
