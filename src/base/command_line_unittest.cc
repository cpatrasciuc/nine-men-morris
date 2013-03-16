// Copyright (c) 2013 Cristian Patrasciuc. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "base/basic_macros.h"
#include "base/command_line.h"
#include "gtest/gtest.h"

namespace base {
namespace {

TEST(CommandLine, Init) {
  const char* const argv[] = {
      "program_name",
      "--foo",
      "--bar=",
      "--baz=qux",
      "-p",
      "-q=",
      "-r=value",
      "--composed-switch=--this-should-not-be-a-switch=",
      "first argument",
      "--",
      "-",
      "--=",
      "-=",
      "\"second argument\""
  };

  CommandLine cmd_line;
  cmd_line.Init(arraysize(argv), argv);

  EXPECT_FALSE(cmd_line.HasSwitch("--missing-switch"));
  EXPECT_FALSE(cmd_line.HasSwitch("first argument"));
  EXPECT_FALSE(cmd_line.HasSwitch("program_name"));
  EXPECT_FALSE(cmd_line.HasSwitch("qux"));
  EXPECT_FALSE(cmd_line.HasSwitch("--"));
  EXPECT_FALSE(cmd_line.HasSwitch("-"));

  EXPECT_EQ("program_name", cmd_line.program_name());

  EXPECT_TRUE(cmd_line.HasSwitch("--foo"));
  EXPECT_TRUE(cmd_line.HasSwitch("--bar"));
  EXPECT_TRUE(cmd_line.HasSwitch("--baz"));
  EXPECT_TRUE(cmd_line.HasSwitch("-p"));
  EXPECT_TRUE(cmd_line.HasSwitch("-q"));
  EXPECT_TRUE(cmd_line.HasSwitch("-r"));
  EXPECT_TRUE(cmd_line.HasSwitch("--composed-switch"));

  EXPECT_EQ("", cmd_line.GetSwitchValue("--foo"));
  EXPECT_EQ("", cmd_line.GetSwitchValue("--bar"));
  EXPECT_EQ("qux", cmd_line.GetSwitchValue("--baz"));
  EXPECT_EQ("", cmd_line.GetSwitchValue("-p"));
  EXPECT_EQ("", cmd_line.GetSwitchValue("-q"));
  EXPECT_EQ("value", cmd_line.GetSwitchValue("-r"));
  EXPECT_EQ("--this-should-not-be-a-switch=",
            cmd_line.GetSwitchValue("--composed-switch"));

  const std::vector<std::string> args = cmd_line.GetArguments();
  ASSERT_EQ(6U, args.size());
  std::vector<std::string>::const_iterator it = args.begin();
  EXPECT_EQ("first argument", *it);
  ++it;
  EXPECT_EQ("--", *it);
  ++it;
  EXPECT_EQ("-", *it);
  ++it;
  EXPECT_EQ("--=", *it);
  ++it;
  EXPECT_EQ("-=", *it);
  ++it;
  EXPECT_EQ("\"second argument\"", *it);

  EXPECT_EQ("program_name --bar --baz=qux "
      "--composed-switch=--this-should-not-be-a-switch= --foo -p -q -r=value "
      "first argument -- - --= -= \"second argument\"",
      cmd_line.GetAsString());
}

TEST(CommandLine, ProgramName) {
  const char first_argument[] = "first argument";
  const char* const argv[] = { first_argument };
  CommandLine cmd_line;
  EXPECT_TRUE(cmd_line.program_name().empty());
  cmd_line.Init(arraysize(argv), argv);
  EXPECT_EQ(first_argument, cmd_line.program_name());

  const char program_name[] = "program name";
  CommandLine cmd_line_with_program_name(program_name);
  EXPECT_EQ(program_name, cmd_line_with_program_name.program_name());
  cmd_line_with_program_name.Init(arraysize(argv), argv);
  EXPECT_EQ(program_name, cmd_line_with_program_name.program_name());
  EXPECT_EQ(first_argument, cmd_line_with_program_name.GetArguments()[0]);

  const char switch_name[] = "--only-switches";
  const char* const only_switches[] = { switch_name };
  CommandLine cmd_line2;
  EXPECT_TRUE(cmd_line2.program_name().empty());
  cmd_line2.Init(arraysize(only_switches), only_switches);
  EXPECT_TRUE(cmd_line2.program_name().empty());
  EXPECT_TRUE(cmd_line2.HasSwitch(switch_name));
}

TEST(CommandLine, Empty) {
  CommandLine cmd_line;
  cmd_line.Init(0, NULL);
  EXPECT_TRUE(cmd_line.GetAsString().empty());
  EXPECT_TRUE(cmd_line.program_name().empty());
  EXPECT_TRUE(cmd_line.GetArguments().empty());
}

TEST(CommandLine, AppendSwitch) {
  const std::string switch_name("--switch");
  const std::string switch_value("value");
  CommandLine cmd_line;
  EXPECT_FALSE(cmd_line.HasSwitch(switch_name));
  cmd_line.AppendSwitch(switch_name);
  EXPECT_TRUE(cmd_line.HasSwitch(switch_name));
  EXPECT_TRUE(cmd_line.GetSwitchValue(switch_name).empty());
  cmd_line.AppendSwitch(switch_name, switch_value, false);
  EXPECT_TRUE(cmd_line.HasSwitch(switch_name));
  EXPECT_TRUE(cmd_line.GetSwitchValue(switch_name).empty());
  cmd_line.AppendSwitch(switch_name, switch_value, true);
  EXPECT_TRUE(cmd_line.HasSwitch(switch_name));
  EXPECT_EQ(switch_value, cmd_line.GetSwitchValue(switch_name));
}

TEST(CommandLine, AppendArgument) {
  const char* const args[] = { "arg1", "-arg2=value", "--arg3" };
  CommandLine cmd_line;
  EXPECT_TRUE(cmd_line.GetArguments().empty());
  for (size_t i = 0; i < arraysize(args); ++i) {
    cmd_line.AppendArgument(args[i]);
  }
  const std::vector<std::string> cmd_arguments = cmd_line.GetArguments();
  ASSERT_EQ(arraysize(args), cmd_arguments.size());
  for (size_t i = 0; i < arraysize(args); ++i) {
    EXPECT_EQ(args[i], cmd_arguments[i]);
  }
}

TEST(CommandLineDeathTest, DEBUG_ONLY_TEST(DoubleInitialization)) {
  ::testing::FLAGS_gtest_death_test_style = "threadsafe";
  CommandLine cmd_line;
  cmd_line.Init(0, NULL);
  ASSERT_DEATH(cmd_line.Init(0, NULL), "");
}

TEST(CommandLineDeathTest, DEBUG_ONLY_TEST(InvalidSwitches)) {
  ::testing::FLAGS_gtest_death_test_style = "threadsafe";
  CommandLine cmd_line;
  ASSERT_DEATH(cmd_line.AppendSwitch("no-initial-dashes"), "");
  ASSERT_DEATH(cmd_line.AppendSwitch("--switch-with-equal-sign=invalid"), "");
}

TEST(CommandLineDeathTest, DEBUG_ONLY_TEST(GetValueOfMissingSwitch)) {
  ::testing::FLAGS_gtest_death_test_style = "threadsafe";
  CommandLine cmd_line;
  ASSERT_DEATH(cmd_line.GetSwitchValue("--not-found-switch"), "");
}

}  // anonymous namespace
}  // namespace base
