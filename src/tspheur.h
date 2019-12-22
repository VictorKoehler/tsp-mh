#include "tspsolution.h"
#include "tspneighmoves.h"


namespace TSPMH {

    template <typename U>
    TSPSolution<U> solutionConstructor(uint dimension, U** matrizAdj);

    template <typename U>
    TSPSolution<U> doubleBridge(TSPSolution<U>* sol);

    template <typename U>
    TSPSolution<U> gils_rvnd(uint d, U **m);
    template <typename U>
    TSPSolution<U> gils_rvnd(uint d, U **m, int Imax, int Iils);


}