#ifndef __ILSHEUR_CLASS__
#define __ILSHEUR_CLASS__

#include "global.h"
#include "levenstein.h"

namespace TSPMH {

    template<class TSPClassWrapper>
    typename TSPClassWrapper::Solution gils(int Imax, int Iils, TSPClassWrapper context) {
        typename TSPClassWrapper::Solution best;
        best.cost = INFINITYLF;

        for (int i = 0; i < Imax; i++) {
            auto bestCandidate = context.construct(); // s'
            auto candidate = bestCandidate; // s
            printf("GILS-RVND loop #%d of %d, started with cost %lf. Best know: %lf\n", i, Imax, candidate.cost, best.cost);
            fflush(stdout);

            ifpertubationtest(typename TSPClassWrapper::Solution pertubation_previous);
            ifpertubationtest(std::vector<std::size_t> pertbdiffs);

            int ccrvnd = 0;
            for (int j = 0; j < Iils; j++, ccrvnd++) {
                context.neighborhood(&candidate);
                #ifdef ENPERTUBTEST
                if (j != 0)
                    pertbdiffs.push_back(LevensteinDistance<typename TSPClassWrapper::Solution>(candidate, pertubation_previous));
                #endif

                if (candidate.cost < bestCandidate.cost) { // s < s'
                    dprintf("RVND loop #%d, new cost update: %lf\n", ccrvnd, candidate.cost);
                    assert(candidate.cost == candidate.update_cost());
                    bestCandidate = candidate; // s' := s
                    j = 0;
                }

                ifpertubationtest(pertubation_previous = candidate);
                candidate = context.pertubation(&bestCandidate);
            }
            
            #ifdef ENPERTUBTEST
            printf("Pertubation Differences:");
            for (auto per : pertbdiffs) {
                printf(" %ld", per);
            }
            printf("\n");
            #endif

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