project (libs-zlib)

add_library(zlib
  adler32.c
  compress.c 
  crc32.c 
  crc32.h
  deflate.c 
  deflate.h
  example.c 
  gzio.c 
  infback.c 
  inffast.c 
  inffast.h
  inflate.c 
  inflate.h
  inftrees.c 
  inftrees.h
  minigzip.c 
  trees.c 
  trees.h
  uncompr.c 
  zlib.h
  zutil.c 
  zutil.h
)

set_target_properties(zlib PROPERTIES PROJECT_LABEL libs-zlib)
set_property(TARGET zlib PROPERTY FOLDER ${LIBS_FOLDER}/zlib)
