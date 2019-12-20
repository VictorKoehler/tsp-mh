#ifndef __TSPTESTS__
#define __TSPTESTS__

#include "tspheur.h"

extern double** matrizAdj;  // matriz de adjacencia
extern uint dimension;      // quantidade total de vertices

void simple_swap_test(MLPSolution &a);

void simple_best_swap_test(MLPSolution &a);

void simple_best_twoopt_test(MLPSolution &a);

void simple_best_reinserion_test(MLPSolution &a, size_t len);

void simple_copy_test(MLPSolution &a);

void simple_test();

#endif