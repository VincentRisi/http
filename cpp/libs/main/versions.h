#ifndef _VERSIONS_H_
#define _VERSIONS_H_ "$Revision: 121 $ $Date: 2005-05-05 15:08:04 +0200 (Thu, 05 May 2005) $"

/* Versions comprise of YMM portion of yyyyMMdd ie. 20030901 is 309 */

#define __LIBTI_VERSION__ 901
#define __STRING__LIBTI_VERSION__ "901"

#ifndef __TIMESTAMP__
#define __TIMESTAMP__ __DATE__ " " __TIME__
#endif

#define COMPILED_VERSION(VERSION) \
static char __COMPILED_VERSION__[] = "{] \r" __FILE__ \
                                     "(" __TIMESTAMP__ ")" \
                                     " " VERSION \
                                     __STRING__LIBTI_VERSION__ \
                                     "\0 [}"

#define _H_VERSION_(n, v) \
static char n##MARKER__[] = "{] \r" __FILE__ ":" __STRING__LIBTI_VERSION__ "(" __TIMESTAMP__  ") " v "\0 [}"
#define HEADER_VERSION(VERSION) _H_VERSION_(_##VERSION, VERSION);

HEADER_VERSION(_VERSIONS_H_)


#endif
