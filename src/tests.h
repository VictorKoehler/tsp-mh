#ifndef __TSPTESTS__
#define __TSPTESTS__

#include "tspheur.h"

void simple_swap_test(Data& data);

void simple_best_swap_test(Data& data);

void simple_best_twoopt_test(Data& data);

void simple_best_reinserion_test(Data& data, size_t len);

void simple_copy_test(Data& data);

#endif