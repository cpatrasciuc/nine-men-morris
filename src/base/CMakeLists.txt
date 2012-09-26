set(BASE_SOURCE_FILES
	basic_macros.h
	debug/stacktrace.cc
	debug/stacktrace.h
	log.cc
	log.h
	ptr/array_storage_policy.h
	ptr/default_ownership_policy.h
	ptr/default_storage_policy.h
	ptr/malloc_storage_policy.h
	ptr/ref_counted.cc
	ptr/ref_counted.h
	ptr/ref_counted_ownership_policy.h
	ptr/ref_ptr.h
	ptr/scoped_array_ptr.h
	ptr/scoped_malloc_ptr.h
	ptr/scoped_ownership_policy.h
	ptr/scoped_ptr.h
	ptr/smart_ptr.h
	string_util.cc
	string_util.h
)

set(BASE_UNITTESTS_SOURCE_FILES
	debug/stacktrace_unittest.cc
	log_unittest.cc
	ptr/ref_ptr_unittest.cc
	ptr/scoped_array_ptr_unittest.cc
	ptr/scoped_malloc_ptr_unittest.cc
	ptr/scoped_ptr_unittest.cc
	ptr/smart_ptr_unittest.cc
	string_util_unittest.cc
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
