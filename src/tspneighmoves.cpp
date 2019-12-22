#include <algorithm>
#include <assert.h>
#include "tspneighmoves.h"

using namespace std;

#define mat sol->matrizAdj
#define it(i) begin() + i

#ifdef vprintfen
#define vprintf dprintf
#else
#define vprintf(...)
#endif


namespace TSPMH {

    #define INFINITYU numeric_limits<U>::max();

    template <typename U>
    inline void swap_apply(TSPSolution<U> *sol, vecit &i, vecit &j, U delta) {
        sol->cost += delta;
        int tmpi = *i;
        *i = *j;
        *j = tmpi;
    }

    template <typename U>
    inline U swap_cost(TSPSolution<U> *sol, const vecit &i, const vecit &j) {
        const int prej = *(j - 1), posj = *(j + 1), prei = *(i - 1), posi = *(i + 1);
        if (posi == *j) {
            return mat[prei][*j] + mat[*i][posj] - mat[prei][*i] - mat[*j][posj];
        }
        return mat[prej][*i] + mat[*i][posj] + mat[prei][*j] + mat[*j][posi]
            - (mat[prej][*j] + mat[*j][posj] + mat[prei][*i] + mat[*i][posi]);
    }

    template <typename U>
    inline U swap_cost(TSPSolution<U> *sol, int o, int p) {
        auto i = sol->it(o), j = sol->it(p);
        if (o > p) return swap_cost(sol, j, i);
        else       return swap_cost(sol, i, j);
    }



    template <typename U>
    void SwapMove<U>::apply(TSPSolution<U> *sol) {
        auto i = sol->it(a), j = sol->it(b);
        swap_apply(sol, i, j, swap_cost(sol, i, j));
    }

    template <typename U>
    void SwapMove<U>::undo(TSPSolution<U> *sol) {
        apply(sol);
    }

    template <typename U>
    U SwapMove<U>::swap_best(TSPSolution<U> *sol, bool auto_push) {
        U delta = INFINITYU;
        vecit bi, bj;
        auto end = sol->end() - 1;
        for (auto i = sol->begin() + 1; i != end; i++) {
            for (auto j = i + 1; j != end; j++) {
                U d = swap_cost(sol, i, j);
                if (d < delta) {
                    bi = i;
                    bj = j;
                    delta = d;
                }
            }
        }

        if (delta < 0) {
            vprintf("Passive SwapMove with delta %.0lf\n", delta);
            if (!auto_push) swap_apply(sol, bi, bj, delta);
            else {
                int i = distance(sol->begin(), bi);
                int j = distance(sol->begin(), bj);
                sol->push_NeighborhoodMove(unique_ptr<NeighborhoodMove<U>>(new SwapMove<U>(i, j)));
            }
        } else vprintf("Positive delta %.0lf => NOP\n", delta);
        return delta;
    }










    template <typename U>
    inline void twoopt_apply(TSPSolution<U> *sol, vecit &i, vecit &j, U delta) {
        sol->cost += delta;
        reverse(i, j+1);
    }

    template <typename U>
    inline U twoopt_cost(TSPSolution<U> *sol, const vecit &i, const vecit &j) {
        const int prej = *(j - 1), posj = *(j + 1), prei = *(i - 1), posi = *(i + 1);
        assert(posi != *j);
        assert(mat[prei][*j] == mat[*j][prei] && mat[*j][posj] == mat[posj][*j]);
        assert(mat[prej][*i] == mat[*i][prej] && mat[*i][posi] == mat[posi][*i]);
        return mat[prei][*j] + mat[*i][posj] - mat[prei][*i] - mat[*j][posj];
    }

    template <typename U>
    inline U twoopt_cost(TSPSolution<U> *sol, int o, int p) {
        auto i = sol->it(o), j = sol->it(p);
        if (o > p) return twoopt_cost(sol, j, i);
        else       return twoopt_cost(sol, i, j);
    }


    template <typename U>
    void TwoOptMove<U>::apply(TSPSolution<U> *sol) {
        auto i = sol->it(a), j = sol->it(b);
        twoopt_apply(sol, i, j, twoopt_cost(sol, i, j));
    }

    template <typename U>
    void TwoOptMove<U>::undo(TSPSolution<U> *sol) {
        apply(sol);
    }

    template <typename U>
    U TwoOptMove<U>::twoopt_best(TSPSolution<U> *sol, bool auto_push) {
        U delta = INFINITYU;
        vecit bi, bj;
        auto end = sol->end() - 1, end2 = end - 1;
        for (auto i = sol->begin() + 1; i != end2; i++) {
            for (auto j = i + 2; j != end; j++) {
                U d = twoopt_cost(sol, i, j);
                if (d < delta) {
                    bi = i;
                    bj = j;
                    delta = d;
                }
            }
        }

        if (delta < 0) {
            vprintf("Passive TwoOptMove with delta %.0lf\n", delta);
            if (!auto_push) twoopt_apply(sol, bi, bj, delta);
            else {
                int i = distance(sol->begin(), bi);
                int j = distance(sol->begin(), bj);
                sol->push_NeighborhoodMove(unique_ptr<NeighborhoodMove<U>>(new TwoOptMove<U>(i, j)));
            }
        } else vprintf("Positive delta %.0lf => NOP\n", delta);
        return delta;
    }











    template <typename U>
    inline void reinsertion_apply(TSPSolution<U> *sol, size_t opos, size_t len, size_t npos, U delta) {
        sol->cost += delta;
        //cout << "Reinserting [" << opos << "..(" << len << ")] to " << npos << " with delta=" << delta << endl;
        vector<int> cpy(sol->it(opos), sol->it(opos + len));
        sol->erase(sol->it(opos), sol->it(opos + len));
        size_t dlen = opos < npos ? len : 0;
        sol->insert(sol->it(npos - dlen), cpy.begin(), cpy.end());
    }

    template <typename U>
    inline U reinsertion_cost(TSPSolution<U> *sol, const vecit &o, size_t len, const vecit &n) {
        const int preo = *(o - 1), poso = *(o + len), pren = *(n - 1), posn = *n,
                bego = *o, endo = *(o + (len - 1));
        return mat[preo][poso] + mat[pren][bego] + mat[endo][posn]
            - (mat[preo][bego] + mat[endo][poso] + mat[pren][posn]);
    }

    template <typename U>
    inline U reinsertion_cost(TSPSolution<U> *sol, size_t opos, size_t len, size_t npos) {
        auto o = sol->it(opos), n = sol->it(npos);
        return reinsertion_cost(sol, o, len, n);
    }


    template <typename U>
    void ReinsertionMove<U>::apply(TSPSolution<U> *sol) {
        reinsertion_apply(sol, opos, len, npos, reinsertion_cost(sol, opos, len, npos));
    }

    template <typename U>
    void ReinsertionMove<U>::undo(TSPSolution<U> *sol) {
        int t = opos;
        opos = npos;
        npos = t;
        if (npos > opos) npos += len;
        else opos -= len;
        //cout << "U:npos=" << npos << ", opos=" << opos << " ";
        apply(sol);
    }

    template <typename U>
    U ReinsertionMove<U>::reinsertion_best(TSPSolution<U> *sol, size_t len, bool auto_push) {
        U delta = INFINITYU;
        vecit bi, bn;
        auto end_max = sol->end() - len;
        for (auto i = sol->begin() + 1; i != end_max; i++) {
            auto maxi = i + len;
            for (auto n = sol->begin() + 1; n != sol->end() - 1; n++) {
                if (i <= n && n <= maxi) continue;
                U d = reinsertion_cost(sol, i, len, n);
                if (d < delta) {
                    bi = i;
                    bn = n;
                    delta = d;
                }
            }
        }

        if (delta < 0) {
            vprintf("Passive ReinsertionMove with delta %.0lf\n", delta);
            int o = distance(sol->begin(), bi);
            int n = distance(sol->begin(), bn);
            if (!auto_push) reinsertion_apply(sol, o, len, n, delta);
            else {
                sol->push_NeighborhoodMove(unique_ptr<NeighborhoodMove<U>>(new ReinsertionMove<U>(o, len, n)));
            }
        } else vprintf("Positive delta %.0lf => NOP\n", delta);
        return delta;
    }

}