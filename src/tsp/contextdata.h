#ifndef __TSPCPDATA_CLASS__
#define __TSPCPDATA_CLASS__

#include <iostream>

namespace TSPMH {

    class TSPContextProblemData {
        public:
        static const uint route_start = 0;
        uint dimension;
        double **matrizAdj;

        TSPContextProblemData(uint d, double** m) : dimension(d), matrizAdj(m) {}
    };
    
}
#endif