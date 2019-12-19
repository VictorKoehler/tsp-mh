#include <numeric>
#include <set>
#include <tuple>
#include <cmath>
#include <algorithm>

#include "tspheur.h"

using namespace std;


const double INFINITYLF = std::numeric_limits<double>::infinity();

#define INITIAL_SUBTOUR_SIZE 3
#define INITIAL_SUBTOUR_ALFA 0.3

#define it(i) begin() + (i)


int _random(int excl_max) {
    return rand() % excl_max;
}

int _random(int incl_min, int excl_max) {
    return rand() % (excl_max - incl_min) + incl_min;
}


TSPSolution solutionConstructor(uint dimension, double** matrizAdj) {
    TSPSolution sol(dimension, matrizAdj);
    vector<int> candidatos(dimension - 1);
    iota(candidatos.begin(), candidatos.end(), 1);

    for (int i = 1; i <= INITIAL_SUBTOUR_SIZE; i++) {
        int r = _random(candidatos.size());
        sol.insert(sol.it(i), candidatos[r]);
        candidatos.erase(candidatos.it(r));
    }
    sol.update_cost();

    while (!candidatos.empty()) {
        set< tuple<double, size_t, size_t> > custoInsercao;
        size_t curr_sz = sol.size()-1;
        int maxtamp = floor(double(curr_sz*candidatos.size())*INITIAL_SUBTOUR_ALFA);
        size_t choose = size_t(_random(maxtamp));

        for (size_t pos = 1; pos < sol.size(); pos++) {
            for (size_t c = 0; c < candidatos.size(); c++) {
                custoInsercao.insert(make_tuple(sol.insertion_cost(candidatos[c], sol.it(pos)), c, pos));
                if (custoInsercao.size() > choose + 1) {
                    custoInsercao.erase(--custoInsercao.end());
                }
            }
        }

        auto cand = *--custoInsercao.end();
        sol.insert_candidate(candidatos[get<1>(cand)], get<2>(cand));
        candidatos.erase(candidatos.it(get<1>(cand)));
    }

    return sol;
}

TSPSolution doubleBridge(TSPSolution* sol) {
    TSPSolution ret(sol->dimension, sol->matrizAdj);
    ret.clear();
    ret.reserve(sol->size());

    int sz4 = sol->size() / 4;
    int pos1 = 1 + _random(sz4);
    int pos2 = pos1 + 1 + _random(sz4);
    int pos3 = pos2 + 1 + _random(sz4);

    ret.insert(ret.end(), sol->begin(), sol->it(pos1));
    ret.insert(ret.end(), sol->it(pos3), sol->end() - 1);
    ret.insert(ret.end(), sol->it(pos2), sol->it(pos3));
    ret.insert(ret.end(), sol->it(pos1), sol->it(pos2));
    ret.push_back(TSPSolution::route_start);

    // TODO: Lazy Calc
    ret.update_cost();

    return ret;
}

TSPSolution gils_rvnd(uint d, double **m) {
    return gils_rvnd(d, m, 50, d >= 150 ? d/2 : d);
}

TSPSolution gils_rvnd(uint d, double **m, int Imax, int Iils) {
    TSPSolution best(d, m);
    best.cost = INFINITYLF;

    for (int i = 0; i < Imax; i++) {
        TSPSolution bestCandidate = solutionConstructor(d, m); // s'
        TSPSolution candidate = bestCandidate; // s
        dprintf("GILS-RVND loop #%d of %d, started with cost %lf. Best know: %lf\n", i, Imax, candidate.cost, best.cost);

        int ccrvnd = 0;
        for (int j = 0; j < Iils; j++, ccrvnd++) {
            vector<int> neighs = { 0, 1, 2, 3, 4 };

            // RVND    
            while (!neighs.empty()) { // s := rvnd
                double c = candidate.cost;
                int ind = _random(neighs.size());
                switch (ind) {
                    case 0: SwapMove::swap_best(&candidate, false); break;
                    case 1: TwoOptMove::twoopt_best(&candidate, false); break;
                    case 2: ReinsertionMove::reinsertion_best(&candidate, 1, false); break;
                    case 3: ReinsertionMove::reinsertion_best(&candidate, 2, false); break;
                    case 4: ReinsertionMove::reinsertion_best(&candidate, 3, false); break;
                }
                if (candidate.cost < c) neighs = { 0, 1, 2, 3, 4 };
                else neighs.erase(neighs.it(ind));
            }

            if (candidate.cost < bestCandidate.cost) { // s < s'
                dprintf("RVND loop #%d, new cost update: %lf\n", ccrvnd, candidate.cost);
                assert(candidate.cost == candidate.update_cost());
                bestCandidate = candidate; // s' := s
                j = 0;
            }
            candidate = doubleBridge(&bestCandidate);
        }

        if (bestCandidate.cost < best.cost) {
            best = bestCandidate;
            assert(bestCandidate.cost == bestCandidate.update_cost());
            dprintf("GILS-RVND loop #%d of %d, with %d iterations updated cost to %lf\n", i, Imax, ccrvnd, best.cost);
        }
    }

    return best;
}