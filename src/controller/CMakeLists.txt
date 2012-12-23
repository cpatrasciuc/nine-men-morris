set(CONTROLLER_SOURCE_FILES
  game_thread.cc
  game_thread.h
)

set(CONTROLLER_UNITTESTS_SOURCE_FILES
  game_thread_unittest.cc
)

include_directories(
  ../
  ../gtest/include
)

link_directories(
  ../gtest
)

add_definitions("-DCONTROLLER_IMPLEMENTATION")

# The main target of this directory
add_library(controller ${CONTROLLER_SOURCE_FILES})

# The unittests for this directory
add_executable(controller_unittests ${CONTROLLER_UNITTESTS_SOURCE_FILES} ../base/test_runner.cc)
target_link_libraries(controller_unittests gtest base game controller)
