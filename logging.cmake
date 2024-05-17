set (LOG_LEVEL "1" CACHE STRING "Log Level")

function (log_debug)
  if ("0" LESS ${LOG_LEVEL})
    return ()
  endif ()
  message (DEBUG " ${ARGN}")
endfunction ()

function (log_info)
  if ("1" LESS ${LOG_LEVEL})
    return ()
  endif ()
  message (STATUS "${ARGN}")
endfunction ()

function (log_warn)
  if ("2" LESS ${LOG_LEVEL})
    return ()
  endif ()
  message (WARNING " ${ARGN}")
endfunction ()

function (log_error)
  message (SEND_ERROR "--err-- ${ARGN}")
endfunction ()

function (log_fatal)
  message (FATAL_ERROR "--err-- ${ARGN}")
endfunction ()

