#include "tspsolution.h"
#include "tspneighmoves.h"

TSPSolution solutionConstructor(uint dimension, double** matrizAdj);

TSPSolution doubleBridge(TSPSolution* sol);

TSPSolution gils_rvnd(uint d, double **m);
TSPSolution gils_rvnd(uint d, double **m, int Imax, int Iils);