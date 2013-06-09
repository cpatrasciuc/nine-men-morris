set(AI_SOURCE_FILES
  ai_algorithm.cc
  ai_algorithm.h
  ai_export.h
  alphabeta/alphabeta.h
  alphabeta/morris_alphabeta.cc
  alphabeta/morris_alphabeta.h
  alphabeta/evaluators.cc
  alphabeta/evaluators.h
  game_state.cc
  game_state.h
  game_state_tree.cc
  game_state_tree.h
  random/random_algorithm.cc
  random/random_algorithm.h
)

set(AI_UNITTESTS_SOURCE_FILES
  alphabeta/alphabeta_unittest.cc
  alphabeta/morris_alphabeta_unittest.cc
  alphabeta/genetic_algorithm.h
  alphabeta/genetic_algorithm_unittest.cc
  game_state_tree_unittest.cc
  game_state_unittest.cc
  random/random_algorithm_unittest.cc
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
target_link_libraries(ai base game rt)

# The unittests for this directory
add_executable(ai_unittests ${AI_UNITTESTS_SOURCE_FILES} ../base/test_runner.cc)
target_link_libraries(ai_unittests gtest base game ai)

set(AI_TRAINER_SOURCE_FILES
  ../base/threading/thread_pool_for_unittests.cc
  alphabeta/morris_alphabeta_trainer.cc
  alphabeta/genetic_algorithm.h
)

add_executable(ai_trainer ${AI_TRAINER_SOURCE_FILES})
target_link_libraries(ai_trainer base game ai)
