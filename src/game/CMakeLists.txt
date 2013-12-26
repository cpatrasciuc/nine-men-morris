set(GAME_SOURCE_FILES
  board.cc
  board.h
  board_location.cc
  board_location.h
  game.cc
  game.h
  game_export.h
  game_listener.cc
  game_listener.h
  game_options.cc
  game_options.h
  game_serializer.cc
  game_serializer.h
  mill_event_listener.cc
  mill_event_listener.h
  mill_events_generator.cc
  mill_events_generator.h
  piece_color.cc
  piece_color.h
  player_action.cc
  player_action.h
)

set(GAME_UNITTESTS_SOURCE_FILES
  board_unittest.cc
  board_location_unittest.cc
  game_listener_unittest.cc
  game_options_unittest.cc
  game_unittest.cc
  game_serializer_unittest.cc
  game_test_helper.cc
  game_test_helper.h
  game_test_helper_unittest.cc
  mill_events_generator_unittest.cc
  piece_color_unittest.cc
  player_action_unittest.cc
)

include_directories(
  ../
  ../gtest/include
)

link_directories(
  ../gtest/
)

add_definitions("-DGAME_IMPLEMENTATION")

# The main target of this directory
add_library(game ${GAME_SOURCE_FILES})
target_link_libraries(game base)

# The unittests for this directory
add_executable(game_unittests ${GAME_UNITTESTS_SOURCE_FILES} ../base/test_runner.cc)
target_link_libraries(game_unittests gtest base game)
