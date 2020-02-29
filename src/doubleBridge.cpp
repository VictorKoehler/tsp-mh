#include "doubleBridge.h"

namespace TSPMH {

    TSPSolution doubleBridge(TSPSolution* sol, TSPMH::vecit sbeg, TSPMH::vecit send, size_t ssize, double ncost) {
        TSPSolution ret(sol->dimension, sol->matrizAdj);
        ret.cost = ncost;
        ret.clear();
        ret.reserve(ssize);

        int sz4 = ssize / 4;
        auto apos1 = sbeg + (_random(sz4)), pos1 = apos1+1;
        auto apos2 = pos1 + (_random(sz4)), pos2 = apos2+1;
        auto apos3 = pos2 + (_random(sz4)), pos3 = apos3+1;
        auto end1 =  send - 1, end2 = end1 - 1;

        ret.insert(ret.end(), sbeg, pos1); // 0
        ret.insert(ret.end(), pos3, end1); // 3
        ret.insert(ret.end(), pos2, pos3); // 2
        ret.insert(ret.end(), pos1, pos2); // 1
        ret.push_back(TSPSolution::route_start); // 4

        ret.cost -= sol->matrizAdj[*apos1][*pos1] + sol->matrizAdj[*apos2][*pos2] + sol->matrizAdj[*apos3][*pos3] + sol->matrizAdj[*end2][*end1];
        ret.cost += sol->matrizAdj[*apos1][*pos3] + sol->matrizAdj[*end2][*pos2] + sol->matrizAdj[*apos3][*pos1] + sol->matrizAdj[*apos2][*end1];
        if (ncost != 0)
            assert(ret.cost == ret.update_cost());
        assert(ret.size() == ssize);

        return ret;
    }

    TSPSolution doubleBridge(TSPSolution* sol) {
        return doubleBridge(sol, sol->begin(), sol->end(), sol->size(), sol->cost);
    }
    
}