#ifndef __TSPHEUR_CLASS__
#define __TSPHEUR_CLASS__

#include "tspsolution.h"
#include "tspneighmoves.h"
#include "tspconstructive.h"


namespace TSPMH {

    TSPSolution doubleBridge(TSPSolution* sol);

    TSPSolution gils_rvnd(uint d, double **m);

    void rvnd(TSPSolution* candidate);
}

namespace CVRPMH {

    CVRPSolution doubleBridge(CVRPSolution* sol);

    CVRPSolution gils_rvnd(std::unique_ptr<LegacyCVRP::Instancia> inst);

    void rvnd(CVRPSolution* candidate);
    
}

#endif