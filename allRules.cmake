
########### cross platform setup ##########
if(CROSS_PLATFORM STREQUAL X86)
	set(CMAKE_C_FLAGS "-Wall -O2 -Os -D_REENTRANT -ffunction-sections -fdata-sections -fPIC")
	set(CMAKE_CXX_FLAGS "-Wall -std=c++11 -O2 -Os -D_REENTRANT -ffunction-sections -fdata-sections -fPIC")
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${CMAKE_FILENAME_MACRO}")
	set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${CMAKE_FILENAME_MACRO}")
	set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -Wl,--gc-sections")
	if (NOT CONF_LIBRARY)
		set(CONF_LIBRARY "static")
	endif ()
	message(STATUS "===> Cross Platform : X86")
	#message(STATUS "allRules.cmake path : >>> ${PROJECT_SOURCE_DIR}")
else()
	message(FATAL_ERROR "Don't set CROSS_PLATFORM")
endif(CROSS_PLATFORM STREQUAL X86)




