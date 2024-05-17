project (libs-main)

add_library(main
  addlist.h
  #amutex.h
  #appendlist.h
  #aqueue.h
  ascii85.cpp ascii85.h
  #athread.cpp athread.h
  #autosemaphore.cpp autosemaphore.h
  base64.cpp base64.h
  #basemacros.h
  #bdatetime.cpp bdatetime.h
  #blackboard.h
  bstring.cpp bstring.h
  cbsocket.cpp cbsocket.h
  #cmdlinearg.cpp cmdlinearg.h
  collection.h
  #config.cpp config.h
  #databuilder2.h
  #databuilderjp.h
  #filehandler.cpp filehandler.h
  #getargs.cpp getargs.h
  #handles.h
  #heapqueue.h
  #hugechar.h
  ini.cpp ini.h
  logfile.cpp logfile.h
  machine.h
  #md5.cpp md5.h
  mutex.cpp mutex.h
  #nedgen.h
  obsocket.cpp obsocket.h
  #padgen.h
  #percent_encoding.cpp percent_encoding.h
  #rdc.cpp rdc.h
  #regexp.cpp regexpc.h
  #splits.cpp splits.h
  #swapbytes.h
  tbuffer.h
  tstring.cpp tstring.h
  #versions.h
  xcept.cpp xcept.h
  xdir.h
  #xml_databuild.cpp xml_databuild.h
  #xmlcore.cpp xmlcore.h
  #xmlmake.cpp xmlmake.h
  #xmlrecord.cpp xmlrecord.h
  #xstring.cpp xstring.h
  z85.cpp z85.h
)

target_link_libraries (main expat zlib)

set_target_properties(main PROPERTIES PROJECT_LABEL libs-main)
set_property(TARGET main PROPERTY FOLDER ${LIBS_FOLDER}/main)
