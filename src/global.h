#ifndef __GLOBAL_MACROS__
#define __GLOBAL_MACROS__

#ifdef ENASSERTS
#undef NDEBUG
#endif
#include <cassert>
#include <numeric>
#include <limits>

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

int pick_random(int excl_max);

int pick_random(int incl_min, int excl_max);

void set_random_seed(uint_fast32_t s);

#endif