CMAKE_MINIMUM_REQUIRED(VERSION 2.8)

set( PACKAGE_VERSION 0.1)

set( CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} )

set(CMAKE_POSITION_INDEPENDENT_CODE ON)

set(Python_ADDITIONAL_VERSIONS 3.4)
find_package(PythonLibs 3 REQUIRED)

message("Include dirs of Python: " ${PYTHON_INCLUDE_DIRS} )
message("Libs of Python: " ${PYTHON_LIBRARIES} )

IF(NOT CMAKE_BUILD_TYPE)
  SET(CMAKE_BUILD_TYPE "DEBUG")
  #SET(CMAKE_BUILD_TYPE "RELEASE")
  #SET(CMAKE_BUILD_TYPE "RELWITHDEBINFO")
  #SET(CMAKE_BUILD_TYPE "MINSIZEREL")
ENDIF()

find_package( Boost 1.58.0 REQUIRED COMPONENTS python3 numpy3)
message("Include dirs of Boost: " ${Boost_INCLUDE_DIRS} )
message("Libs of Boost: " ${Boost_LIBRARIES} )

INCLUDE_DIRECTORIES("${Boost_INCLUDE_DIRS}" ${PYTHON_INCLUDE_DIRS})
SET(Boost_USE_STATIC_LIBS ON)
SET(Boost_USE_MULTITHREADED ON)
SET(Boost_USE_STATIC_RUNTIME ON)
#LIST( APPEND BOOST_COMPONENTS python3)

set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR})

#set(SRC_DIR ${CMAKE_CURRENT_SOURCE_DIR}/src)

include_directories(${CMAKE_CURRENT_SOURCE_DIR})
#include_directories(${SRC_DIR})

file(GLOB SRC
    *.h
    *.cpp
)

message("Source files: " ${SRC} )

ADD_LIBRARY(svx SHARED pysvx.h ${SRC} svx.cpp )
TARGET_LINK_LIBRARIES(svx ${PYTHON_LIBRARIES} ${Boost_LIBRARIES})

IF(CMAKE_COMPILER_IS_GNUCXX)
  ADD_DEFINITIONS("-Wall")
ELSE()
  MESSAGE(FATAL_ERROR "CMakeLists.txt has not been tested/written for your compiler.")
ENDIF()

find_program(PYTHON "python3")

set(SETUP_PY_IN "${CMAKE_CURRENT_SOURCE_DIR}/setup.py.in")
set(SETUP_PY    "${CMAKE_CURRENT_BINARY_DIR}/setup.py")
set(DEPS      svx)
set(OUTPUT      "${CMAKE_CURRENT_BINARY_DIR}/build")

configure_file(${SETUP_PY_IN} ${SETUP_PY})

add_custom_command(OUTPUT ${OUTPUT}
		COMMAND ${CMAKE_COMMAND} -E copy
			"${CMAKE_CURRENT_SOURCE_DIR}/libsvx.so"
			"${CMAKE_CURRENT_BINARY_DIR}/SLICsupervoxels/libsvx.so"
		COMMAND ${CMAKE_COMMAND} -E copy
			"${CMAKE_CURRENT_SOURCE_DIR}/SLICsupervoxels/__init__.py"
			"${CMAKE_CURRENT_BINARY_DIR}/SLICsupervoxels/__init__.py"
	       DEPENDS ${DEPS})

add_custom_target(target ALL DEPENDS ${OUTPUT})

##install(CODE "execute_process(COMMAND ${PYTHON} ${SETUP_PY} install)")
