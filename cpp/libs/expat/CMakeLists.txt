project (libs-expat)

add_library(expat
  xmldef.h
  xmlparse.h 
  xmlparse.c 
  xmlrole.c 
  xmlrole.h
  xmltok.c 
  xmltok.h
)

set_target_properties(expat PROPERTIES PROJECT_LABEL libs-expat)
set_property(TARGET expat PROPERTY FOLDER ${LIBS_FOLDER}/expat)
