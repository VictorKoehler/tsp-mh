#ifndef __TSPTESTS__
#define __TSPTESTS__

#include "tsp/heur.h"

extern double** matrizAdj;  // matriz de adjacencia
extern uint dimension;      // quantidade total de vertices

void simple_swap_test();

void simple_best_swap_test();

void simple_best_twoopt_test();

void simple_best_reinserion_test(size_t len);

void simple_copy_test();

#endif