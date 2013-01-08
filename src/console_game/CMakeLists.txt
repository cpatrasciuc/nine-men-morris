set(CONSOLE_GAME_SOURCE_FILES
  board_renderer.cc
  board_renderer.h
  command_handler.cc
  command_handler.h
  console_game.cc
  console_game.h
  help_command_handler.cc
  help_command_handler.h
  player_actions_command_handler.cc
  player_actions_command_handler.h
  save_game_command_handler.cc
  save_game_command_handler.h
)

include_directories(
  ../
)

add_executable(console_game ${CONSOLE_GAME_SOURCE_FILES} console_game_main.cc)
target_link_libraries(console_game base game)
