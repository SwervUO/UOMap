cmake_minimum_required(VERSION 3.18)

project(UOMap VERSION 0.01 LANGUAGES CXX)

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

add_subdirectory(uodata)
add_subdirectory(utility)

add_executable(UOMap
	main.cpp

)

target_link_libraries(UOMap
	PUBLIC
		utility
		uodata
)

target_include_directories(UOMap
	PRIVATE
		uodata
		utility
)
