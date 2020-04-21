#ifndef __CVRPCONSTRUCTIVE_CLASS__
#define __CVRPCONSTRUCTIVE_CLASS__

#include "../tsp/constructive.h"
#include "solution.h"


namespace CVRPMH {

    class GreedyDummyConstructor : public TSPMH::TSPSolutionConstructor<CVRPSolution> {
        public:
        LegacyCVRP::Instancia *inst;
        CVRPContextProblemData *data;

        GreedyDummyConstructor(LegacyCVRP::Instancia *inst, CVRPContextProblemData *data) : inst(inst), data(data) { }

        CVRPSolution construct();
    };

    class BestInsertionConstructor : public TSPMH::TSPSolutionConstructor<CVRPSolution> {
        public:
        LegacyCVRP::Instancia *inst;
        CVRPContextProblemData *data;

        BestInsertionConstructor(LegacyCVRP::Instancia *inst, CVRPContextProblemData *data) : inst(inst), data(data) { }

        CVRPSolution construct();
    };
}

#endif