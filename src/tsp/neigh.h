#ifndef __TSPNEIGHBORHOOD_CLASS__
#define __TSPNEIGHBORHOOD_CLASS__

#include "global.h"

namespace TSPMH {

    class TSPSolution;
    class NeighborhoodMove {
        public:
        virtual void apply(TSPSolution *) {};
        virtual void undo(TSPSolution *) {};
        virtual void best(TSPSolution *, bool) {};
        virtual ~NeighborhoodMove() {}
    };
    
}

#endif
