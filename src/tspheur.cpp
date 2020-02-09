#include <numeric>
#include <set>
#include <tuple>
#include <cmath>
#include <algorithm>

#include "tspheur.h"
#include "cvrpneighmoves.h"

using namespace std;


#define INITIAL_SUBTOUR_SIZE 3
#define INITIAL_SUBTOUR_ALFA 0.3

#define it(i) begin() + (i)


namespace TSPMH {


    const double INFINITYLF = std::numeric_limits<double>::infinity();

    inline TSPSolution doubleBridge(TSPSolution* sol, TSPMH::vecit sbeg, TSPMH::vecit send, size_t ssize, double ncost) {
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

    template<class T=TSPSolution>
    inline T doubleBridge(T* sol) {
        return doubleBridge(sol, sol->begin(), sol->end(), sol->size(), sol->cost);
    }

    template<>
    inline CVRPMH::CVRPSolution doubleBridge(CVRPMH::CVRPSolution* sol) {
        return CVRPMH::doubleBridge(sol);
    }




    template<class T=TSPSolution>
    void rvnd(T* candidate);

    template<>
    void rvnd(TSPSolution* candidate) {
        vector<int> neighs = { 0, 1, 2, 3, 4 };

        while (!neighs.empty()) { // s := rvnd
            double c = candidate->cost;
            int ind = _random(neighs.size());
            switch (ind) {
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

    template<>
    void rvnd(CVRPMH::CVRPSolution* candidate) {
        return CVRPMH::rvnd(candidate);
    }



    template<class TSPClass, class TSPConstr>
    TSPClass gen_gils_rvnd(int Imax, int Iils, TSPConstr constr) {
        TSPClass best;
        best.cost = INFINITYLF;

        for (int i = 0; i < Imax; i++) {
            TSPClass bestCandidate = constr.construct(); // s'
            TSPClass candidate = bestCandidate; // s
            printf("GILS-RVND loop #%d of %d, started with cost %lf. Best know: %lf\n", i, Imax, candidate.cost, best.cost);
            fflush(stdout);

            int ccrvnd = 0;
            for (int j = 0; j < Iils; j++, ccrvnd++) {
                rvnd<TSPClass>(&candidate);

                if (candidate.cost < bestCandidate.cost) { // s < s'
                    dprintf("RVND loop #%d, new cost update: %lf\n", ccrvnd, candidate.cost);
                    assert(candidate.cost == candidate.update_cost());
                    bestCandidate = candidate; // s' := s
                    j = 0;
                }
                candidate = doubleBridge<TSPClass>(&bestCandidate);
            }

            if (bestCandidate.cost < best.cost) {
                best = bestCandidate;
                assert(bestCandidate.cost == bestCandidate.update_cost());
                dprintf("GILS-RVND loop #%d of %d, with %d iterations updated cost to %lf\n", i, Imax, ccrvnd, best.cost);
            }
        }

        return best;
    }

    TSPSolution gils_rvnd(uint d, double **m) {
        return gen_gils_rvnd<TSPSolution, BestInsertionConstructor>(50, d >= 150 ? d/2 : d, BestInsertionConstructor(d, m));
    }
}

namespace CVRPMH {

    CVRPSolution doubleBridge(CVRPSolution* sol) {
        CVRPSolution r;
        sol->updateSubRoutes();
        r.reserve(sol->size());
        r.cpy(*sol);
        for (int i = 0; i < sol->vehicles; i++) {
            auto s = SubRoute(sol, i);
            if (s.size() < 8) {
                r.insert(r.end(), s.begin(), s.end()-1);
            } else {
                auto n = TSPMH::doubleBridge(&s, s.begin(), s.end(), s.size(), 0); // TODO: FIX
                r.insert(r.end(), n.begin(), n.end()-1);
            }
        }
        r.push_back(CVRPSolution::route_start);
        r.update_cost();
        r.updateSubRoutes();
        assert(r.size() == r.dimension + r.vehicles);
        return r;
    }

    void rvnd(CVRPSolution* candidate) {
        for (int i = 0; i < candidate->vehicles; i++) {
            auto s = SubRoute(candidate, i);
            s.cost = 0;
            TSPMH::rvnd<TSPMH::TSPSolution>(static_cast<TSPMH::TSPSolution*>(&s));
            candidate->cost += s.cost;
        }
        assert(candidate->checkSolution());

        vector<int> neighs = { 0, 1, 2, 3 };

        while (!neighs.empty()) { // s := rvnd
            double c = candidate->cost;
            int ind = TSPMH::_random(neighs.size());
            switch (ind) {
                case 0: SwapMove::swap_best(candidate); break;
                case 1: ReinsertionMove::reinsertion_best(candidate, 1); break;
                case 2: ReinsertionMove::reinsertion_best(candidate, 2); break;
                case 3: ReinsertionMove::reinsertion_best(candidate, 3); break;
            }
            assert(candidate->checkSolution());
            if (candidate->cost < c) neighs = { 0, 1, 2, 3 };
            else neighs.erase(neighs.it(ind));
        }
        assert(candidate->size() == candidate->dimension + candidate->vehicles);
    }

    CVRPSolution gils_rvnd(std::unique_ptr<LegacyCVRP::Instancia> inst) {
        const int d = inst->dimension;
        return TSPMH::gen_gils_rvnd<CVRPSolution, GreedyDummyConstructor>(50, d >= 150 ? d/2 : d, GreedyDummyConstructor(inst.get()));
    }

}