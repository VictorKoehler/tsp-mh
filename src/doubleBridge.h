
#include "tspheur.h"

namespace TSPMH {

    TSPSolution doubleBridge(TSPSolution* sol);
    
    TSPSolution doubleBridge(TSPSolution* sol, TSPMH::vecit sbeg, TSPMH::vecit send, size_t ssize, double ncost);

}