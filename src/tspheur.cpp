#include <numeric>
#include <set>
#include <tuple>
#include <cmath>
#include <algorithm>

#include "tspheur.h"

using namespace std;


#define INITIAL_SUBTOUR_SIZE 3
#define INITIAL_SUBTOUR_ALFA 0.3

#define it(i) begin() + (i)


namespace MLPMH {


    const double INFINITYLF = std::numeric_limits<double>::infinity();


    int _random(int excl_max) {
        return rand() % excl_max;
    }

    int _random(int incl_min, int excl_max) {
        return rand() % (excl_max - incl_min) + incl_min;
    }


    MLPSolution solutionConstructor(uint dimension, double** matrizAdj) {
        MLPSolution sol(dimension, matrizAdj);
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
                    custoInsercao.insert(make_tuple(sol.tsp_insertion_cost(candidatos[c], sol.it(pos)), c, pos));
                    if (custoInsercao.size() > choose + 1) {
                        custoInsercao.erase(--custoInsercao.end());
                    }
                }
            }

            auto cand = *--custoInsercao.end();
            sol.insert(sol.it(get<2>(cand)), candidatos[get<1>(cand)]);
            candidatos.erase(candidatos.it(get<1>(cand)));
        }
        sol.update_cost();

        return sol;
    }

    #define add(i) _add(i, sol->matrizAdj, sol)
    MLPSolution doubleBridge(MLPSolution* sol, bool concatenate) {
        MLPSolution ret(sol->dimension, sol->matrizAdj);
        ret.subseqConcatenation = sol->subseqConcatenation;
        ret.clear();
        ret.reserve(sol->size());

        int sz4 = sol->size() / 4;
        const auto npos1 =         1 + _random(sz4);
        const auto npos2 = npos1 + 1 + _random(sz4);
        const auto npos3 = npos2 + 1 + _random(sz4);
        const auto nend1 = sol->size() - 1, nend2 = nend1 - 1;

        const auto pos1 = sol->it(npos1);
        const auto pos2 = sol->it(npos2);
        const auto pos3 = sol->it(npos3);
        const auto end1 = sol->it(nend1), end2 = sol->it(nend2);

        ret.insert(ret.end(), sol->begin(), pos1); // 0
        ret.insert(ret.end(), pos3, end1); // 3
        ret.insert(ret.end(), pos2, pos3); // 2
        ret.insert(ret.end(), pos1, pos2); // 1
        ret.push_back(MLPSolution::route_start); // 4

        if (concatenate) {
            const auto last = sol->dimension;
            const auto subseq = sol->subseqConcatenation;

            // [0..*] [1..*] [2..*] [3..*] [4]
            // [0..*] [3..*] [2..*] [1..*] [4]
            auto seg0 = subcint(0, npos1-1), seg1 = subcint(npos1, npos2-1), seg2 = subcint(npos2, npos3-1), seg3 = subcint(npos3, nend1-1), seg4 = subcint(nend1, nend1);
            auto d = seg0.add(seg3).add(seg2).add(seg1).add(seg4);
            ret.cost = d.c;
            ret.duration = d.t;
        } else {
            ret.update_cost();
        }

        assert(ret.update_cost_duration_same());

        return ret;
    }
    #undef add

    MLPSolution gils_rvnd(uint d, double **m) {
        return gils_rvnd(d, m, 50, d >= 150 ? d/2 : d);
    }

    MLPSolution gils_rvnd(uint d, double **m, int Imax, int Iils) {
        MLPSolution best(d, m);
        best.alloc_subseqConcatenation();
        best.cost = INFINITYLF;

        for (int i = 0; i < Imax; i++) {
            MLPSolution bestCandidate = solutionConstructor(d, m); // s'
            bestCandidate.subseqConcatenation = best.subseqConcatenation;
            MLPSolution candidate = bestCandidate; // s
            dprintf("GILS-RVND loop #%d of %d, started with cost %lf. Best know: %lf\n", i, Imax, candidate.cost, best.cost);

            int ccrvnd = 0;
            for (int j = 0; j < Iils; j++) {
                ccrvnd++;
                vector<int> neighs = { 0, 1, 2, 3, 4 };
                candidate.update_subseqConcatenation();

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
                    if (candidate.cost < c) {
                        neighs = { 0, 1, 2, 3, 4 };
                        candidate.update_subseqConcatenation();
                    }
                    else neighs.erase(neighs.it(ind));
                }

                if (candidate.cost < bestCandidate.cost) { // s < s'
                    dprintf("RVND loop #%d, new cost update: %lf\n", ccrvnd, candidate.cost);
                    assert(candidate.update_cost_duration_same());
                    bestCandidate = candidate; // s' := s
                    j = 0;
                    candidate = doubleBridge(&bestCandidate, true);
                } else {
                    candidate = doubleBridge(&bestCandidate, false);
                }
            }

            if (bestCandidate.cost < best.cost) {
                best = bestCandidate;
                assert(bestCandidate.update_cost_duration_same());
                dprintf("GILS-RVND loop #%d of %d, with %d iterations updated cost to %lf\n", i, Imax, ccrvnd, best.cost);
            }
        }

        best.dealloc_subseqConcatenation();
        return best;
    }

}