#include "tspsolution.h"
#include "tspneighmoves.h"

MLPSolution solutionConstructor(uint dimension, double** matrizAdj);

MLPSolution doubleBridge(MLPSolution* sol, bool concatenate);

MLPSolution gils_rvnd(uint d, double **m);
MLPSolution gils_rvnd(uint d, double **m, int Imax, int Iils);