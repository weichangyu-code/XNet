cmake_minimum_required(VERSION 3.0.0)
project(XNet VERSION 0.1.0)

if (NOT CROSS_PLATFORM)
	set(CROSS_PLATFORM X86)
endif ()
include(allRules.cmake)

# 17可以触发读写锁优化性能
set(CMAKE_CXX_STANDARD 17)

add_subdirectory(./src)
add_subdirectory(./test)
