project (libs-jsoncpp)

set (jsoncpp_source
  json/allocator.h
  json/assertions.h
  json/autolink.h
  json/config.h
  json/features.h
  json/forwards.h
  json/json.h
  json/reader.h
  json/value.h
  json/version.h
  json/writer.h
  json_tool.h
  json_reader.cpp
  json_valueiterator.inl
  json_value.cpp
  json_writer.cpp
  json_databuild.cpp
  json_databuild.h
  json_databuild2.cpp
  json_databuildjp.cpp
  )

add_library(jsoncpp ${jsoncpp_source})
target_link_libraries (jsoncpp main)

set_target_properties(jsoncpp PROPERTIES PROJECT_LABEL libs-jsoncpp)
set_property(TARGET jsoncpp PROPERTY FOLDER ${LIBS_FOLDER}/jsoncpp)

