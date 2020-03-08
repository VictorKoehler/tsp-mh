#ifndef __GLOBAL_MACROS__
#define __GLOBAL_MACROS__

#ifdef ENASSERTS
#undef NDEBUG
#endif
#include <cassert>
#include <numeric>

#ifdef ENDEBUG
#define ENVERBOSE 1
#define ENPERTUBTEST 1
#define ifdebug(...) __VA_ARGS__
#else
#define ifdebug(...)
#endif

#ifdef ENVERBOSE
#define dprintf(...) printf(__VA_ARGS__)
#else
#define dprintf(...)
#endif

#ifdef ENPERTUBTEST
#define ifpertubationtest(...) __VA_ARGS__
#else
#define ifpertubationtest(...)
#endif

const double INFINITYLF = std::numeric_limits<double>::infinity();

#endif