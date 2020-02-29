#ifndef __ILSHEUR_CLASS__
#define __ILSHEUR_CLASS__

#include "global.h"

namespace TSPMH {

    template<class TSPClassWrapper, class TSPClassWrapperSolution>
    TSPClassWrapperSolution gils(int Imax, int Iils, TSPClassWrapper context) {
        TSPClassWrapperSolution best;
        best.cost = INFINITYLF;

        for (int i = 0; i < Imax; i++) {
            auto bestCandidate = context.construct(); // s'
            auto candidate = bestCandidate; // s
            printf("GILS-RVND loop #%d of %d, started with cost %lf. Best know: %lf\n", i, Imax, candidate.cost, best.cost);
            fflush(stdout);

            int ccrvnd = 0;
            for (int j = 0; j < Iils; j++, ccrvnd++) {
                context.neighborhood(&candidate);

                if (candidate.cost < bestCandidate.cost) { // s < s'
                    dprintf("RVND loop #%d, new cost update: %lf\n", ccrvnd, candidate.cost);
                    assert(candidate.cost == candidate.update_cost());
                    bestCandidate = candidate; // s' := s
                    j = 0;
                }
                candidate = context.pertubation(&bestCandidate);
            }

            if (bestCandidate.cost < best.cost) {
                best = bestCandidate;
                assert(bestCandidate.cost == bestCandidate.update_cost());
                printf("GILS-RVND loop #%d of %d, with %d iterations updated cost to %lf\n", i, Imax, ccrvnd, best.cost);
            } else {
                printf("GILS-RVND loop #%d of %d, with %d iterations didn't improved, stopped with cost %lf\n", i, Imax, ccrvnd, bestCandidate.cost);
            }
        }

        return best;
    }

}
#endif