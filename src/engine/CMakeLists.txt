set(ENGINE_SOURCE_FILES
  game_controller.cc
  game_controller.h
  game_thread.cc
  game_thread.h
  player.cc
  player.h
)

set(ENGINE_UNITTESTS_SOURCE_FILES
  game_thread_unittest.cc
)

include_directories(
  ../
  ../gtest/include
)

link_directories(
  ../gtest
)

add_definitions("-DENGINE_IMPLEMENTATION")

# The main target of this directory
add_library(engine ${ENGINE_SOURCE_FILES})

# The unittests for this directory
add_executable(engine_unittests ${ENGINE_UNITTESTS_SOURCE_FILES} ../base/test_runner.cc)
target_link_libraries(engine_unittests gtest base game engine)
