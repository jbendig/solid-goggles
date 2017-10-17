CMAKE_MINIMUM_REQUIRED(VERSION 3.2)

FIND_PROGRAM(CCACHE_FOUND ccache)
IF(CCACHE_FOUND)
	SET_PROPERTY(GLOBAL PROPERTY RULE_LAUNCH_COMPILE ${CCACHE})
	SET_PROPERTY(GLOBAL PROPERTY RULE_LAUNCH_LINK ${CCACHE})
ENDIF()

PROJECT(solid-goggles)

IF(${CMAKE_SYSTEM_NAME} STREQUAL "Linux")
	SET(LINUX True)
ENDIF()

SET(CMAKE_INCLUDE_CURRENT_DIR ON)
SET(CMAKE_AUTOMOC ON)

FIND_PACKAGE(Qt5Widgets REQUIRED)

SET(SOURCE_FILES
	src/main.cpp
	src/MainWindow.cpp
	extern/QVideoDecoder.cpp
)

ADD_EXECUTABLE(solid-goggles ${SOURCE_FILES})
SET_TARGET_PROPERTIES(solid-goggles PROPERTIES COMPILE_FLAGS "-Wall -std=c++14")
QT5_USE_MODULES(solid-goggles Widgets)

IF(LINUX)
	TARGET_INCLUDE_DIRECTORIES(solid-goggles PRIVATE "${CMAKE_SOURCE_DIR}/extern" "/usr/include/ffmpeg")
	TARGET_LINK_LIBRARIES(solid-goggles avutil avcodec avformat avfilter swscale OpenCL)
	ADD_CUSTOM_TARGET(run DEPENDS solid-goggles WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}" COMMAND "${CMAKE_BINARY_DIR}/solid-goggles" || true)
	ADD_CUSTOM_TARGET(debug DEPENDS solid-goggles WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}" COMMAND "gdb" "${CMAKE_BINARY_DIR}/solid-goggles" "-ex" "run" || true)
	ADD_CUSTOM_TARGET(memcheck DEPENDS solid-goggles WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}" COMMAND "valgrind" "--tool=memcheck" "--leak-check=full" "${CMAKE_BINARY_DIR}/solid-goggles" || true)
	ADD_CUSTOM_TARGET(callgrind DEPENDS solid-goggles WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}" COMMAND "valgrind" "--tool=callgrind" "--dump-instr=yes" "--collect-jumps=yes" "--simulate-cache=yes" "${CMAKE_BINARY_DIR}/solid-goggles" || true)
ENDIF()
