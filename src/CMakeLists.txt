cmake_minimum_required (VERSION 2.8)

project(nine-men-morris)

if (RELEASE)
  set(cflags "-Werror -pthread -fno-exceptions -fno-strict-aliasing -Wall -Wno-unused-parameter -Wno-missing-field-initializers -fvisibility=hidden -pipe -fPIC -O2 -fdata-sections -ffunction-sections")
  set(only_cxx_flags "-fno-rtti -fno-threadsafe-statics -fvisibility-inlines-hidden -Wsign-compare")
  set(only_c_flags "")
  set(ldflags "-pthread -Wl,-z,noexecstack -fPIC -Wl,--threads -Wl,--thread-count=4 -Wl,--icf=none -Wl,-O1 -Wl,--as-needed -Wl,--gc-sections")
else()
  set(cflags "-Werror -pthread -fno-exceptions -fno-strict-aliasing -Wall -Wno-unused-parameter -Wno-missing-field-initializers -fvisibility=hidden -pipe -fPIC -O0 -g")
  set(only_cxx_flags "-fno-rtti -fno-threadsafe-statics -fvisibility-inlines-hidden -Wsign-compare")
  set(only_c_flags "")
  set(ldflags "-pthread -Wl,-z,noexecstack -fPIC -Wl,--threads -Wl,--thread-count=4 -Wl,--icf=none")
endif()

set(CMAKE_C_FLAGS "${cflags} ${only_c_flags}")
set(CMAKE_CXX_FLAGS "${cflags} ${only_cxx_flags}")

if (RELEASE)
else()
  add_definitions("-DENABLE_LOGGING")
endif()

add_subdirectory(base)
add_subdirectory(gtest)

