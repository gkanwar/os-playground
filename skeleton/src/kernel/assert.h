#ifndef ASSERT_H
#define ASSERT_H

#include "panic.h"

#ifdef NDEBUG
#define assert(x,s)
#else
#define MACRO_TO_STR_IN(x) #x
#define MACRO_TO_STR(x) MACRO_TO_STR_IN(x)
#define FILE_CONTEXT __FILE__ ":" MACRO_TO_STR(__LINE__)
#define assert(x,s) if (!(x)) { panic(FILE_CONTEXT "\nAssert '" #x "' failed\nReason " #s); }
#endif

#endif
