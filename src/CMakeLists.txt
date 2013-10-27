cmake_minimum_required (VERSION 2.8)

project(nine-men-morris)

# Relative link directories are relative to the source directory.
cmake_policy(SET CMP0015 NEW)

if(RELEASE)
  set(CMAKE_BUILD_TYPE "Release")
else()
  set(CMAKE_BUILD_TYPE "Debug")
endif()

if(RELEASE)
  set(cflags "-Werror -pthread -fno-strict-aliasing -Wall -Wno-unused-parameter -Wno-missing-field-initializers -fvisibility=hidden -pipe -fPIC -O2 -fdata-sections -ffunction-sections")
  set(only_cxx_flags "-fno-threadsafe-statics -fvisibility-inlines-hidden -Wsign-compare")
  set(only_c_flags "")
  set(ldflags "-pthread -Wl,-z,noexecstack -fPIC -Wl,-O1 -Wl,--as-needed -Wl,--gc-sections")
else()
  set(cflags "-Werror -pthread -fno-strict-aliasing -Wall -Wno-unused-parameter -Wno-missing-field-initializers -fvisibility=hidden -pipe -fPIC -O0 -g --coverage")
  set(only_cxx_flags " -fno-threadsafe-statics -fvisibility-inlines-hidden -Wsign-compare")
  set(only_c_flags "")
  set(ldflags "-pthread -Wl,-z,noexecstack -fPIC")
endif()

if(${CMAKE_C_COMPILER} MATCHES ".*gcc")
  set(cflags "${cflags} -Wno-deprecated -Wno-pragmas")
endif()

if(BUILD_SHARED_LIBS)
  add_definitions("-DGTEST_LINKED_AS_SHARED_LIBRARY=1")
  set(ldflags "-rdynamic ${ldflags}")
endif()

# TODO(compile flags): Add -fno-rtti and -fno-exceptions
set(CMAKE_C_FLAGS "${cflags} ${only_c_flags}")
set(CMAKE_CXX_FLAGS "${cflags} ${only_cxx_flags}")

set(CMAKE_EXE_LINKER_FLAGS ${ldflags})
set(CMAKE_MODULE_LINKER_FLAGS ${ldflags})
set(CMAKE_SHARED_LINKER_FLAGS ${ldflags})

if(RELEASE)
  add_definitions("-DNDEBUG")
  add_definitions("-DRELEASE_MODE")
else()
  add_definitions("-DDEBUG_MODE")
  add_definitions("-DENABLE_LOGGING")
  add_definitions("-DENABLE_DCHECK")
endif()

add_subdirectory(base)

add_subdirectory(game)

add_subdirectory(ai)

add_subdirectory(console_game)

if(NOT DEFINED ENV{TRAVIS})
  add_subdirectory(graphics)
endif()

# Hack to make all gtest symbols visible; by default we compile with -fvisibility=hidden
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fvisibility=default")
add_subdirectory(gtest)

