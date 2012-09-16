set(BASE_SOURCE_FILES
	basic_macros.h
	log.cc
	log.h
)

set(BASE_UNITTESTS_SOURCE_FILES
	log_unittest.cc
)

include_directories(
	../
	../gtest/include
)

link_directories(
	../gtest/
)

add_library(base ${BASE_SOURCE_FILES})

add_executable(base_unittests ${BASE_UNITTESTS_SOURCE_FILES} test_runner.cc)
target_link_libraries(base_unittests gtest base)
