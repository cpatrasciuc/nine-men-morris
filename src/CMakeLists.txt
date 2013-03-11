cmake_minimum_required (VERSION 2.8)

project(nine-men-morris)

# Relative link directories are relative to the source directory.
cmake_policy(SET CMP0015 NEW)

if(RELEASE)
  set(cflags "-Werror -pthread -fno-exceptions -fno-strict-aliasing -Wall -Wno-unused-parameter -Wno-missing-field-initializers -fvisibility=hidden -pipe -fPIC -O2 -fdata-sections -ffunction-sections")
  set(only_cxx_flags "-fno-rtti -fno-threadsafe-statics -fvisibility-inlines-hidden -Wsign-compare")
  set(only_c_flags "")
  set(ldflags "-pthread -Wl,-z,noexecstack -fPIC -Wl,--threads -Wl,--thread-count=4 -Wl,--icf=none -Wl,-O1 -Wl,--as-needed -Wl,--gc-sections")
else()
  set(cflags "-Werror -pthread -fno-exceptions -fno-strict-aliasing -Wall -Wno-unused-parameter -Wno-missing-field-initializers -fvisibility=hidden -pipe -fPIC -O0 -g --coverage")
  set(only_cxx_flags "-fno-rtti -fno-threadsafe-statics -fvisibility-inlines-hidden -Wsign-compare")
  set(only_c_flags "")
  set(ldflags "-pthread -Wl,-z,noexecstack -fPIC -Wl,--threads -Wl,--thread-count=4 -Wl,--icf=none")
endif()

set(CMAKE_C_FLAGS "${cflags} ${only_c_flags}")
set(CMAKE_CXX_FLAGS "${cflags} ${only_cxx_flags}")

if(BUILD_SHARED_LIBS)
  add_definitions("-DGTEST_LINKED_AS_SHARED_LIBRARY=1")
endif()

if(RELEASE)
  add_definitions("-DNDEBUG")
  add_definitions("-DRELEASE_MODE")
else()
  add_definitions("-DDEBUG_MODE")
  add_definitions("-DENABLE_LOGGING")
endif()

add_subdirectory(base)

add_subdirectory(game)

#add_subdirectory(engine)

add_subdirectory(console_game)

add_subdirectory(graphics)

# Hack to make all gtest symbols visible; by default we compile with -fvisibility=hidden
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fvisibility=default")
add_subdirectory(gtest)

