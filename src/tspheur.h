#include "tspsolution.h"
#include "tspneighmoves.h"


namespace TSPMH {

    StackedTSPSolution solutionConstructor(Data& data);

    StackedTSPSolution doubleBridge(StackedTSPSolution* sol);

    StackedTSPSolution gils_rvnd(Data& data);
    StackedTSPSolution gils_rvnd(Data& data, int Imax, int Iils);

}