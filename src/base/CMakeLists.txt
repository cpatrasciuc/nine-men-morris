set(BASE_SOURCE_FILES
	base_export.h
	basic_macros.h
	bind.h
	bind_policy.h
	binders.h
	callable.h
	debug/stacktrace.cc
	debug/stacktrace.h
	location.cc
	location.h
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
	ptr/supports_weak_ref.h
	ptr/weak_ownership_policy.h
	ptr/weak_ptr.h
	string_util.cc
	string_util.h
	threading/atomic.h
	threading/lock.cc
	threading/lock.h
	threading/task.cc
	threading/task.h
	threading/thread.cc
	threading/thread.h
	threading/thread_specific.cc
	threading/thread_specific.h
)

set(BASE_UNITTESTS_SOURCE_FILES
	bind_unittest.cc
	callable_unittest.cc
	debug/stacktrace_unittest.cc
	location_unittest.cc
	log_unittest.cc
	ptr/ref_ptr_unittest.cc
	ptr/scoped_array_ptr_unittest.cc
	ptr/scoped_malloc_ptr_unittest.cc
	ptr/scoped_ptr_unittest.cc
	ptr/smart_ptr_unittest.cc
	ptr/weak_ptr_unittest.cc
	string_util_unittest.cc
	threading/atomic_unittest.cc
	threading/task_unittest.cc
	threading/thread_unittest.cc
	threading/thread_specific_unittest.cc
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
