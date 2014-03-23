// From http://gareus.org/wiki/embedding_resources_in_executables
// By Robin Gareus.
//
#if defined(__APPLE__)
#include <mach-o/ldsyms.h>
#include <mach-o/getsect.h>
#define EXTLD(NAME) \
  /*extern const unsigned char _section$__DATA__ ## NAME [];*/
/*#define LDVAR(NAME) _section$__DATA__ ## NAME*/
/*#define LDVAR(NAME) getsectdata("__DATA", "__" #NAME, NULL)*/
/*#define LDLEN(NAME) (getsectbyname("__DATA", "__" #NAME)->size)*/

#elif defined(__WIN32__)  /* mingw */
#define EXTLD(NAME) \
  extern const unsigned char binary_ ## NAME ## _start[]; \
  extern const unsigned char binary_ ## NAME ## _end[];
#define LDVAR(NAME) \
  binary_ ## NAME ## _start
#define LDLEN(NAME) \
  ((binary_ ## NAME ## _end) - (binary_ ## NAME ## _start))

#else /* gnu/linux ld */
#define EXTLD(NAME) \
  extern const unsigned char _binary_ ## NAME ## _start[]; \
  extern const unsigned char _binary_ ## NAME ## _end[];
#define LDVAR(NAME) \
  _binary_ ## NAME ## _start
#define LDLEN(NAME) \
  ((_binary_ ## NAME ## _end) - (_binary_ ## NAME ## _start))
#endif
