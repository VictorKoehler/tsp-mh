#include <numeric>
#include <set>
#include <tuple>
#include <cmath>
#include <algorithm>

#include "../ils.h"
#include "../tsp/heur.h"
#include "../tsp/doubleBridge.h"
#include "neighmoves.h"
#include "heur.h"
#include "constructive.h"

#define CVRPPOOL_ENABLED

#ifdef CVRPPOOL_ENABLED
#include "cvrppool.h"
#endif

using namespace std;

#define it(i) begin() + (i)


namespace CVRPMH {


    CVRPW_BI_RVND_DB::Solution CVRPW_BI_RVND_DB::construct() {
        return BestInsertionConstructor(inst).construct();
    }

    CVRPSolution doubleBridge(CVRPSolution* sol) {
        CVRPSolution r;
        sol->updateRoutes();
        r.reserve(sol->size());
        r.cpy(*sol);
        for (auto s : sol->getRoutes()) {
            if (s.size() < 8) {
                r.insert(r.end(), s.begin(), s.end()-1);
            } else {
                auto n = TSPMH::doubleBridge(&s, s.begin(), s.end(), s.size(), 0); // TODO: FIX
                r.insert(r.end(), n.begin(), n.end()-1);
            }
        }
        const auto rs = CVRPSolution::route_start;
        r.push_back(rs);

        // if (*(sol->end() - 2) == rs) {
            
        //     set< pair<double, TSPMH::vecit> > custoInsercao;
        //     size_t choose = max(3LU, size_t(random() % min(long(r.size()/3), 50l)));

        //     for (TSPMH::vecit zit = r.begin() + 2; zit < r.end() - 1; zit++) {
        //         if (*(zit-1) != rs && *zit != rs) {
        //             custoInsercao.insert(make_pair(r.insertion_cost(rs, zit), zit));
        //             if (custoInsercao.size() > choose) {
        //                 custoInsercao.erase(--custoInsercao.end());
        //             }
        //         }
        //     }

        //     auto rcand = *--custoInsercao.end(), bcand = *custoInsercao.begin();
        //     auto& cand = bcand.first < 0 ? bcand : rcand;
        //     if (cand.first < 0 || random() % 3 > 0) {
        //         dprintf("Inserted a pertubation-zero in %ld, delta cost %lf\n", cand.second-r.begin(), cand.first);
        //         r.pop_back();
        //         r.insert(cand.second, rs);
        //     }
        // }

        r.update_cost();
        r.updateRoutes();
        assert(r.size() == r.dimension + r.vehicles);
        return r;
    }

    CVRPW_BI_RVND_DB::Solution CVRPW_BI_RVND_DB::pertubation(Solution* sol) {
        return doubleBridge(sol);
    }

    #ifdef CVRPPOOL_ENABLED
    CVRPMH::CVRPPool* pool;
    #endif

    template<bool CVRPPoolEnabled>
    inline double vndsub(CVRPSolution* candidate, CVRPRoute& s) {
        if (s.size() > 3) {
            TSPMH::rvnd(static_cast<TSPMH::TSPSolution*>(&s));
            candidate->cost += s.cost;
            
            if constexpr (CVRPPoolEnabled) {
                s.update_cost(); // TODO: Refactor
                auto sb = s.begin(), se = s.end();
                pool->insert(sb, se, s.cost);
                return s.cost;
            }
        }
        return s.update_cost(); // TODO: Refactor
    }

    template<bool CVRPPoolEnabled>
    inline void rvndb(CVRPSolution* candidate) {
        double costssum = 0;
        for (auto s : candidate->getRoutes()) {
            s.cost = 0;
            costssum += vndsub<CVRPPoolEnabled>(candidate, s);
        }
        assert(candidate->checkSolution(true));
        assert(candidate->cost == costssum);


        vector<int> neighs = { 0, 1, 2, 3 };

        while (!neighs.empty()) { // s := rvnd
            int ind = TSPMH::_random(neighs.size());
            MovementResult mr;
            switch (neighs[ind]) {
                case 0: mr = SwapMove::swap_best(candidate); break;
                case 1: mr = ReinsertionMove::reinsertion_best(candidate, 1); break;
                case 2: mr = ReinsertionMove::reinsertion_best(candidate, 2); break;
                case 3: mr = ReinsertionMove::reinsertion_best(candidate, 3); break;
            }
            assert(candidate->checkSolution(true));

            if (mr) {
                neighs = { 0, 1, 2, 3 };
                assert(mr->first != mr->second);
                vndsub<CVRPPoolEnabled>(candidate, mr->first);
                vndsub<CVRPPoolEnabled>(candidate, mr->second);
            } else {
                neighs.erase(neighs.it(ind));
            }
        }
        assert(candidate->size() == candidate->dimension + candidate->vehicles);
    }

    void rvnd(CVRPSolution* candidate) {
        rvndb<false>(candidate);
    }

    void CVRPW_BI_RVND_DB::localsearch(Solution* sol) {
        #ifdef CVRPPOOL_ENABLED
        rvndb<true>(sol);
        #else
        rvndb<false>(sol);
        #endif
    }

    CVRPSolution gils_rvnd(std::unique_ptr<LegacyCVRP::Instancia> inst) {
        const int d = inst->dimension;

        #ifdef CVRPPOOL_ENABLED
        CVRPMH::CVRPPool pq(inst->vehicles, inst->dimension);
        pool = &pq;
        #endif

        CVRPW_BI_RVND_DB::Solution r = TSPMH::gils<CVRPW_BI_RVND_DB>(50, d >= 150 ? d/2 : d, CVRPW_BI_RVND_DB(inst.get()));

        #ifdef CVRPPOOL_ENABLED
        // #ifndef NOTDEBUG
        // cout << "Current Best: ";
        // for (auto s : r.getSubRoutes()) {
        //     s.update_cost(); // TODO: Refactor
        //     auto sb = s.begin(), se = s.end();
        //     pool->insert(sb, se, s.cost);
        //     VectorHash v;
        //     auto h = v(make_pair(vector<int>(sb, se), s.cost));
        //     cout << s.cost << "(" << h << ") ";
        // }
        // cout << "\n";
        // #endif

        auto p = pq.commit().first;
        r.assign(p.begin(), p.end());
        r.update_cost();
        r.updateRoutes();
        #endif
        
        return r;
    }
}
