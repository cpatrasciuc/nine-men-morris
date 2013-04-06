set(AI_SOURCE_FILES
  ai_algorithm.cc
  ai_algorithm.h
  ai_export.h
)

set(AI_UNITTESTS_SOURCE_FILES
)

include_directories(
  ../
  ../gtest/include
)

link_directories(
  ../gtest/
)

add_definitions("-DAI_IMPLEMENTATION")

# The main target of this directory
add_library(ai ${AI_SOURCE_FILES})
target_link_libraries(ai base game)

# The unittests for this directory
add_executable(ai_unittests ${AI_UNITTESTS_SOURCE_FILES} ../base/test_runner.cc)
target_link_libraries(ai_unittests gtest base game ai)
