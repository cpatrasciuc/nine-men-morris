set(GAME_SOURCE_FILES
	game_export.h
)

set(GAME_UNITTESTS_SOURCE_FILES
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

# The unittests for this directory
add_executable(game_unittests ${GAME_UNITTESTS_SOURCE_FILES} ../base/test_runner.cc)
target_link_libraries(game_unittests gtest base)
