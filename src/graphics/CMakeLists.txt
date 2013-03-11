set(GRAPHICS_SOURCE_FILES
  graphics_export.h
  ogre_app.cc
  ogre_app.h
)

set(GRAPHICS_UNITTESTS_SOURCE_FILES
)

include_directories(
  ../
  ../gtest/include
)

link_directories(
  ../gtest/
)

add_definitions("-DGRAPHICS_IMPLEMENTATION")

# The main target of this directory
add_library(graphics ${GRAPHICS_SOURCE_FILES})
target_link_libraries(graphics base game)


