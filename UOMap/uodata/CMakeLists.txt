cmake_minimum_required(VERSION 3.18)

project(uodata VERSION 0.01 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)

if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE Release)
endif()

if (WIN32)
set(CMAKE_CXX_FLAGS_RELEASE "/O2")
elseif (APPLE)
set(CMAKE_CXX_FLAGS_RELEASE "-Os")
else ()
set(CMAKE_CXX_FLAGS_RELEASE "-O2")
endif()

add_library(uodata
	STATIC
		mapblock.cpp
		uopfile.cpp
		uomap.cpp
)
target_include_directories(uodata
	PRIVATE
		../utility
)
target_compile_options(uodata
	PRIVATE
		$<$<PLATFORM_ID:Windows>:/W3>

		$<$<PLATFORM_ID:Linux>:-Wno-unused-result>
)

target_compile_definitions(uodata
	PRIVATE
		$<$<PLATFORM_ID:Windows>:_LIB>
		$<$<PLATFORM_ID:Windows>:_UNICODE>
		$<$<PLATFORM_ID:Windows>:UNICODE>
		NDEBUG
)

