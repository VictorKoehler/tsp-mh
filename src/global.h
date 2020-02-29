#ifndef __GLOBAL_MACROS__
#define __GLOBAL_MACROS__

#ifdef ENASSERTS
#undef NDEBUG
#endif
#include <cassert>
#include <numeric>

#ifdef ENDEBUG
#define ENVERBOSE 1
#define ifdebug(...) __VA_ARGS__
#else
#define ifdebug(...)
#endif

#ifdef ENVERBOSE
#define dprintf(...) printf(__VA_ARGS__)
#else
#define dprintf(...)
#endif

const double INFINITYLF = std::numeric_limits<double>::infinity();

#endif