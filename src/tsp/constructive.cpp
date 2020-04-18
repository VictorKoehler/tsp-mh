#include <numeric>
#include <set>
#include <tuple>
#include <cmath>
#include <algorithm>

#include "heur.h"
#include <random>

using namespace std;


#define INITIAL_SUBTOUR_SIZE 3
#define INITIAL_SUBTOUR_ALFA 0.3

#define it(i) begin() + (i)

namespace TSPMH {

    minstd_rand simple_rand;

    int _random(int excl_max) {
        return int(simple_rand()) % excl_max;
    }

    int _random(int incl_min, int excl_max) {
        return _random(excl_max - incl_min) + incl_min;
    }

    void _random_seed(uint_fast32_t s) {
        simple_rand.seed(s);
    }

    StackedTSPSolution BestInsertionConstructor::construct() {
        StackedTSPSolution sol(dimension, matrizAdj);
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

}
