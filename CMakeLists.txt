cmake_minimum_required(VERSION 3.20)
project (http)

include (logging.cmake)

option (USE_FOLDERS "Use Folders" ON)
set_property (GLOBAL PROPERTY USE_FOLDERS ${USE_FOLDERS})

if (WIN32)
  if (CMAKE_SIZEOF_VOID_P EQUAL 8)
    set (WIN_SIZE 64 CACHE STRING "Window Size")
  else ()  
    set (WIN_SIZE 32 CACHE STRING "Window Size")
  endif ()
endif ()

if (UNIX)
  exec_program (uname OUTPUT_VARIABLE SYSTEM_NAME)
  set (SYSTEM_NAME "${SYSTEM_NAME}" CACHE INTERNAL "")
  if (SYSTEM_NAME STREQUAL "Linux")
    set (LINUX ON)
  elseif (SYSTEM_NAME STREQUAL "FreeBSD")
    set (FREEBSD ON)
  endif ()
endif ()

if (LINUX)
  log_info ("LINUX")
  set (switches " -g -fPIC -Wno-write-strings -fpermissive")
  list (APPEND CMAKE_C_FLAGS "${switches}")
  list (APPEND CMAKE_CXX_FLAGS "${switches}")
  string (REPLACE ";" " " CMAKE_C_FLAGS   ${CMAKE_C_FLAGS})
  string (REPLACE ";" " " CMAKE_CXX_FLAGS ${CMAKE_CXX_FLAGS})
  set (switches " -pthread")
  list (APPEND CMAKE_EXE_LINKER_FLAGS "${switches}")
  list (APPEND CMAKE_MODULE_LINKER_FLAGS "${switches}")
  list (APPEND CMAKE_SHARED_LINKER_FLAGS "${switches}")
  string (REPLACE ";" " " CMAKE_EXE_LINKER_FLAGS    ${CMAKE_EXE_LINKER_FLAGS})
  string (REPLACE ";" " " CMAKE_MODULE_LINKER_FLAGS ${CMAKE_MODULE_LINKER_FLAGS})
  string (REPLACE ";" " " CMAKE_SHARED_LINKER_FLAGS ${CMAKE_SHARED_LINKER_FLAGS})
elseif (WIN32)
  log_info ("WIN32")
  add_definitions (
    -D_CRT_SECURE_NO_WARNINGS
    -D_SCL_SECURE_NO_WARNINGS
    )
  string (REPLACE "/MD" "/MT" CMAKE_C_FLAGS_DEBUG            ${CMAKE_C_FLAGS_DEBUG})
  string (REPLACE "/MD" "/MT" CMAKE_C_FLAGS_MINSIZEREL       ${CMAKE_C_FLAGS_MINSIZEREL})
  string (REPLACE "/MD" "/MT" CMAKE_C_FLAGS_RELEASE          ${CMAKE_C_FLAGS_RELEASE})
  string (REPLACE "/MD" "/MT" CMAKE_C_FLAGS_RELWITHDEBINFO   ${CMAKE_C_FLAGS_RELWITHDEBINFO})
  string (REPLACE "/MD" "/MT" CMAKE_CXX_FLAGS_DEBUG          ${CMAKE_CXX_FLAGS_DEBUG})
  string (REPLACE "/MD" "/MT" CMAKE_CXX_FLAGS_MINSIZEREL     ${CMAKE_CXX_FLAGS_MINSIZEREL})
  string (REPLACE "/MD" "/MT" CMAKE_CXX_FLAGS_RELEASE        ${CMAKE_CXX_FLAGS_RELEASE})
  string (REPLACE "/MD" "/MT" CMAKE_CXX_FLAGS_RELWITHDEBINFO ${CMAKE_CXX_FLAGS_RELWITHDEBINFO})
endif ()

#if (WIN32)
#  set (PYTHON_HOME           "C:/Python310")
#  set (PYTHON_SCRIPTS        "${PYTHON_HOME}/Scripts")
#  set (PYTHON_INCLUDE_PATH   "${SUPPORT_DIR}/python310/include")
#  link_directories (${SUPPORT_DIR}/python310/libs)
#  set (PYTHON_LIBRARIES       python310 python310_d)
#  set (PYTHON_LIB            "${PYTHON_HOME}/Lib")
#  set (PYTHON_BUILD_LIBRARIES ${PYTHON_LIBRARIES})
#  set (PYTHON_EXECUTABLE     "${PYTHON_HOME}/python.exe")
#elseif (LINUX)
#  set (PYTHON_HOME            "$ENV{PYTHONHOME}")
#  set (PYTHON_INCLUDE_PATH    "${PYTHON_HOME}/include/python3.7m")
#  set (PYTHON_LIBRARIES       "${PYTHON_HOME}/lib/python3.7/config-3.7m-x86_64-linux-gnu/libpython3.7m.a")
#  list (APPEND PYTHON_LIBRARIES ${LIB_UTIL})
#  set (PYTHON_LIB             "${PYTHON_HOME}/Lib")
#  set (PYTHON_PIC_LIBRARIES   "${PYTHON_HOME}/lib/libpython3.7m.so")
#  set (PYTHON_BUILD_LIBRARIES ${PYTHON_LIBRARIES})
#  set (PYTHON_EXECUTABLE      "${PYTHON_HOME}/bin/python3")
#  set (PYTHON_CONFIG_DIR      "${PYTHON_HOME}/lib/python3.7/config-3.7m-x86_64-linux-gnu")
#endif ()

#set (JTOOLS_DIR  ${CMAKE_SOURCE_DIR}/jtools)
#set (PYTOOLS_DIR ${CMAKE_SOURCE_DIR}/pytools)
set (C_DIR  ${CMAKE_SOURCE_DIR}/c)

set(EXECUTABLE_OUTPUT_PATH "${CMAKE_BINARY_DIR}/bin"   CACHE FILEPATH "Single output directory for building all executeables")
set(LIBRARY_OUTPUT_PATH    "${CMAKE_BINARY_DIR}/lib"   CACHE FILEPATH "Single output directory for building all libraries")

add_subdirectory (cpp)
