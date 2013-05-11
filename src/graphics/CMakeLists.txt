project(3d_game)

set(CMAKE_MODULE_PATH "/usr/lib64/OGRE/cmake/;${CMAKE_MODULE_PATH}")

find_package(OGRE REQUIRED)

find_package(OIS REQUIRED)

if(NOT OIS_FOUND)
	message(SEND_ERROR "Failed to find OIS.")
endif()

set(GRAPHICS_SOURCE_FILES
  board_renderer.cc
  board_renderer.h
  board_utils.cc
  board_utils.h
  game_state.cc
  game_state.h
  graphics_export.h
  ogre_app.cc
  ogre_app.h
  playing_state.cc
  playing_state.h
)

set(GRAPHICS_UNITTESTS_SOURCE_FILES
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
target_link_libraries(graphics base game ${OGRE_LIBRARIES} ${OIS_LIBRARIES})

add_executable(graphics_exe graphics_temp_main.cc)
target_link_libraries(graphics_exe graphics)


