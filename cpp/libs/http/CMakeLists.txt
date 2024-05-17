project (libs-http)

set (source
  httpserver.cpp
  httpserver.h
  httprequest.cpp
  httprequest.h
  httpxcept.cpp
  httpxcept.h
  )

ADD_LIBRARY(http
   ${source}
)
target_link_libraries (http jsoncpp)

set_target_properties(http PROPERTIES PROJECT_LABEL libs-http)
set_property(TARGET http PROPERTY FOLDER ${LIBS_FOLDER}/http)
