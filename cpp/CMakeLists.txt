project (c)

set (LIBS_SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/libs)
set (LIBS_BINARY_DIR ${CMAKE_CURRENT_SOURCE_DIR}/libs/build)

include_directories (
  ${LIBS_SOURCE_DIR}/expat
  ${LIBS_SOURCE_DIR}/zlib
  ${LIBS_SOURCE_DIR}/main
  ${LIBS_SOURCE_DIR}/http
  ${LIBS_SOURCE_DIR}/jsoncpp
  )

#set (BUILD_SQL_DIR  ${JTOOLS_DIR}/out/sql)
#set (BUILD_IDL2_DIR ${JTOOLS_DIR}/out/idl2)

add_subdirectory (libs)
