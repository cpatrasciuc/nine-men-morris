project(3d_game)

set(CMAKE_MODULE_PATH "/usr/lib64/OGRE/cmake/;${CMAKE_MODULE_PATH}")

find_package(OGRE REQUIRED)

find_package(OIS REQUIRED)

if(NOT OIS_FOUND)
	message(SEND_ERROR "Failed to find OIS.")
endif()

set(GRAPHICS_SOURCE_FILES
  ai_player.cc
  ai_player.h
  animation_controller.cc
  animation_controller.h
  board_view.cc
  board_view.h
  camera_controller.cc
  camera_controller.h
  confirmation_menu_state.cc
  confirmation_menu_state.h
  game_over_state.cc
  game_over_state.h
  game_state.cc
  game_state.h
  graphics_export.h
  human_player.cc
  human_player.h
  main_menu_state.cc
  main_menu_state.h
  menu_state.cc
  menu_state.h
  new_game_menu_state.cc
  new_game_menu_state.h
  ogre_app.cc
  ogre_app.h
  player_configuration.h
  playing_state.cc
  playing_state.h
  player_delegate.cc
  player_delegate.h
  post_effects.cc
  post_effects.h
  selection_listener.cc
  selection_listener.h
)

set(IN_GAME_TESTS_SOURCE_FILES
  ../game/game_test_helper.cc
  ../game/game_test_helper.h
  ai_player_in_game_test.cc
  animation_controller_test.cc
  board_view_test.cc
  board_view_test_utils.cc
  board_view_test_utils.h
  camera_controller_test.cc
  confirmation_menu_state_test.cc
  game_state_test.cc
  in_game_test.cc
  in_game_test.h
  menu_state_test.cc
  ois_test_utils.cc
  ois_test_utils.h
  out_of_process_test.cc
  out_of_process_test.h
  post_effects_test.cc
)

include_directories(
  ../
  ../gtest/include
  ${OIS_INCLUDE_DIRS}
  ${OGRE_INCLUDE_DIRS}
)

link_directories(
  ../gtest/
)

add_definitions("-DGRAPHICS_IMPLEMENTATION")

# The main target of this directory
add_library(graphics ${GRAPHICS_SOURCE_FILES})
target_link_libraries(graphics base game ai ${OGRE_LIBRARIES} ${OIS_LIBRARIES})

add_executable(graphics_exe graphics_temp_main.cc)
target_link_libraries(graphics_exe graphics)

add_executable(in_game_tests ${IN_GAME_TESTS_SOURCE_FILES} in_game_tests_main.cc)
target_link_libraries(in_game_tests graphics gtest)

