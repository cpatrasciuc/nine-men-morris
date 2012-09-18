set(BASE_SOURCE_FILES
	basic_macros.h
	debug/stacktrace.cc
	debug/stacktrace.h
	log.cc
	log.h
)

set(BASE_UNITTESTS_SOURCE_FILES
	debug/stacktrace_unittest.cc
	log_unittest.cc
)

include_directories(
	../
	../gtest/include
)

link_directories(
	../gtest/
)

add_definitions("-DBASE_IMPLEMENTATION")

# The main target of this directory
add_library(base ${BASE_SOURCE_FILES})

# This is always a shared library because it's used by the stacktrace test
# and we want to have all the symbols available.
add_library(stacktrace_test_helper SHARED 
  debug/stacktrace_test_helper.cc
  debug/stacktrace_test_helper.h
)
target_link_libraries(stacktrace_test_helper base)

# The unittests for this directory
add_executable(base_unittests ${BASE_UNITTESTS_SOURCE_FILES} test_runner.cc)
target_link_libraries(base_unittests gtest base stacktrace_test_helper)
