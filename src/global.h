#ifndef __TSPGLOBAL__
#define __TSPGLOBAL__

#define likely(x)      __builtin_expect(!!(x), 1)
#define unlikely(x)    __builtin_expect(!!(x), 0)

#ifndef NDEBUG
#define Verbosity 5
#define dbgprintf(...) printf(__VA_ARGS__)
#define DEBUGSTATMENT(a) a
#else
#define Verbosity 0
#define dbgprintf(...)
#define DEBUGSTATMENT(a)
#endif

#endif
