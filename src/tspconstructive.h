#ifndef __TSPCONSTRUCTIVE_CLASS__
#define __TSPCONSTRUCTIVE_CLASS__

#include "tspsolution.h"
#include "cvrpsolution.h"

namespace TSPMH {

    inline int _random(int excl_max) {
        return rand() % excl_max;
    }

    inline int _random(int incl_min, int excl_max) {
        return _random(excl_max - incl_min) + incl_min;
    }

    template <typename T>
    class TSPSolutionConstructor {
        public:
        virtual T construct() = 0;
    };


    class BestInsertionConstructor : public TSPSolutionConstructor<StackedTSPSolution> {
        public:
        uint dimension;
        double** matrizAdj;

        BestInsertionConstructor(uint d, double** m) : dimension(d), matrizAdj(m) { }

        StackedTSPSolution construct();
    };
}

namespace CVRPMH {

    class GreedyDummyConstructor : public TSPMH::TSPSolutionConstructor<CVRPSolution> {
        public:
        LegacyCVRP::Instancia *inst;

        GreedyDummyConstructor(LegacyCVRP::Instancia *inst) : inst(inst) { }

        CVRPSolution construct();
    };

    class BestInsertionConstructor : public TSPMH::TSPSolutionConstructor<CVRPSolution> {
        public:
        LegacyCVRP::Instancia *inst;

        BestInsertionConstructor(LegacyCVRP::Instancia *inst) : inst(inst) { }

        CVRPSolution construct();
    };
}

#endif