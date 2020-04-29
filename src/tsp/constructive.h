#ifndef __TSPCONSTRUCTIVE_CLASS__
#define __TSPCONSTRUCTIVE_CLASS__

#include "solution.h"

namespace TSPMH {

    template <typename T>
    class TSPSolutionConstructor {
        public:
        virtual T construct() = 0;
    };


    class BestInsertionConstructor : public TSPSolutionConstructor<StackedTSPSolution> {
        public:
        TSPContextProblemData dataholder;

        BestInsertionConstructor(uint d, double** m) : dataholder(d, m) { }

        StackedTSPSolution construct();
    };
}

#endif