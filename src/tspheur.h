#ifndef __TSPHEUR_CLASS__
#define __TSPHEUR_CLASS__

#include "tspsolution.h"
#include "tspneighmoves.h"
#include "tspconstructive.h"


namespace TSPMH {

    class TSPW_BI_RVND_DB {
        public:
        typedef TSPSolution Solution;
        
        uint dimension;
        double** matrizAdj;

        TSPW_BI_RVND_DB(uint dimension, double** matrizAdj) : dimension(dimension), matrizAdj(matrizAdj) { }

        Solution construct();

        void localsearch(Solution* sol);

        Solution pertubation(Solution* sol);
    };

    void rvnd(TSPSolution* candidate);

    TSPSolution gils_rvnd(uint d, double **m);

}

#endif