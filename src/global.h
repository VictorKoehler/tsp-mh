#ifndef __TSPGLOBAL__
#define __TSPGLOBAL__

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
