set(CONSOLE_GAME_SOURCE_FILES
  ai_player.cc
  ai_player.h
  board_renderer.cc
  board_renderer.h
  command_handler.cc
  command_handler.h
  console_game.cc
  console_game.h
  help_command_handler.cc
  help_command_handler.h
  human_player.cc
  human_player.h
  player.cc
  player.h
  player_actions_command_handler.cc
  player_actions_command_handler.h
  save_game_command_handler.cc
  save_game_command_handler.h
)

set(CONSOLE_GAME_UNITTESTS_SOURCE_FILES
  board_renderer_unittest.cc
  console_game_unittest.cc
  help_command_handler_unittest.cc
  player_actions_command_handler_unittest.cc
  ../game/game_test_helper.cc
  ../game/game_test_helper.h
)

include_directories(
  ../
  ../gtest/include
)

add_definitions("-DCONSOLE_GAME_IMPLEMENTATION")

add_library(console_game ${CONSOLE_GAME_SOURCE_FILES})
target_link_libraries(console_game base game ai)

add_executable(console_game_unittests
    ${CONSOLE_GAME_UNITTESTS_SOURCE_FILES} ../base/test_runner.cc)
target_link_libraries(console_game_unittests gtest console_game)

add_executable(console_game_exe console_game_main.cc)
target_link_libraries(console_game_exe console_game)
set_target_properties(console_game_exe PROPERTIES OUTPUT_NAME console_game)
