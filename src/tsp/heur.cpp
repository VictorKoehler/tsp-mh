#include <numeric>
#include <set>
#include <tuple>
#include <cmath>
#include <algorithm>

#include "heur.h"
#include "neighmoves.h"
#include "doubleBridge.h"
#include "../ils.h"

using namespace std;

#define it(i) begin() + (i)

namespace TSPMH {


    TSPW_BI_RVND_DB::Solution TSPW_BI_RVND_DB::construct() {
        return BestInsertionConstructor(dimension, matrizAdj).construct();
    }

    void TSPW_BI_RVND_DB::localsearch(Solution* sol) {
        rvnd(sol);
    }

    TSPW_BI_RVND_DB::Solution TSPW_BI_RVND_DB::pertubation(Solution* sol) {
        return doubleBridge(sol);
    }


    void rvnd(TSPSolution* candidate) {
        vector<int> neighs = { 0, 1, 2, 3, 4 };

        while (!neighs.empty()) { // s := rvnd
            double c = candidate->cost;
            int ind = _random(neighs.size());
            switch (neighs[ind]) {
                case 0: SwapMove::swap_best(candidate, false); break;
                case 1: TwoOptMove::twoopt_best(candidate, false); break;
                case 2: ReinsertionMove::reinsertion_best(candidate, 1, false); break;
                case 3: ReinsertionMove::reinsertion_best(candidate, 2, false); break;
                case 4: ReinsertionMove::reinsertion_best(candidate, 3, false); break;
            }
            if (candidate->cost < c) neighs = { 0, 1, 2, 3, 4 };
            else neighs.erase(neighs.it(ind));
        }
    }


    TSPSolution gils_rvnd(uint d, double **m) {
        return gils<TSPW_BI_RVND_DB>(50, d >= 150 ? d/2 : d, TSPW_BI_RVND_DB(d, m));
    }
}
