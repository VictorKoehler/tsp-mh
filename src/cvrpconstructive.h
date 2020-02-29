#ifndef __CVRPCONSTRUCTIVE_CLASS__
#define __CVRPCONSTRUCTIVE_CLASS__

#include "tspconstructive.h"
#include "cvrpsolution.h"


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