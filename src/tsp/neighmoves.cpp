#include <algorithm>
#include "neighmoves.h"


#define mat sol->matrizAdj
#define it(i) begin() + i

#ifdef vprintfen
#define vprintf dprintf
#else
#define vprintf(...) {}
#endif


namespace TSPMH {
    
    uint tsp_nmcc_swap = 0, tsp_nmcc_twoopt = 0, tsp_nmcc_reinsertion = 0;
    
    inline void swap_apply(TSPSolution *sol, vecit &i, vecit &j, double delta) {
        tsp_nmcc_swap++;
        sol->cost += delta;
        int tmpi = *i;
        *i = *j;
        *j = tmpi;
    }

    inline double swap_cost(TSPSolution *sol, const vecit &i, const vecit &j) {
        const int prej = *(j - 1), posj = *(j + 1), prei = *(i - 1), posi = *(i + 1);
        if (posi == *j) {
            return mat[prei][*j] + mat[*i][posj] - mat[prei][*i] - mat[*j][posj];
        }
        return mat[prej][*i] + mat[*i][posj] + mat[prei][*j] + mat[*j][posi]
            - (mat[prej][*j] + mat[*j][posj] + mat[prei][*i] + mat[*i][posi]);
    }

    inline double swap_cost(TSPSolution *sol, int o, int p) {
        auto i = sol->it(o), j = sol->it(p);
        if (o > p) return swap_cost(sol, j, i);
        else       return swap_cost(sol, i, j);
    }



    void SwapMove::apply(TSPSolution *sol) {
        auto i = sol->it(a), j = sol->it(b);
        swap_apply(sol, i, j, swap_cost(sol, i, j));
    }

    void SwapMove::undo(TSPSolution *sol) {
        apply(sol);
    }

    double SwapMove::swap_best(TSPSolution *sol, bool auto_push) {
        double delta = INFINITYLF;
        vecit bi, bj;
        auto end = sol->end() - 1;
        if (sol->size() < 4)
            return delta;
        for (auto i = sol->begin() + 1; i != end; i++) {
            for (auto j = i + 1; j != end; j++) {
                double d = swap_cost(sol, i, j);
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
                int i = std::distance(sol->begin(), bi);
                int j = std::distance(sol->begin(), bj);
                static_cast<StackedTSPSolution*>(sol)->push_NeighborhoodMove(std::unique_ptr<NeighborhoodMove>(new SwapMove(i, j)));
            }
        } else vprintf("Positive delta %.0lf => NOP\n", delta);
        return delta;
    }










    inline void twoopt_apply(TSPSolution *sol, vecit &i, vecit &j, double delta) {
        tsp_nmcc_twoopt++;
        sol->cost += delta;
        std::reverse(i, j+1);
    }

    inline double twoopt_cost(TSPSolution *sol, const vecit &i, const vecit &j) {
        const int prej = *(j - 1), posj = *(j + 1), prei = *(i - 1), posi = *(i + 1);
        assert(posi != *j);
        assert(mat[prei][*j] == mat[*j][prei] && mat[*j][posj] == mat[posj][*j]);
        assert(mat[prej][*i] == mat[*i][prej] && mat[*i][posi] == mat[posi][*i]);
        return mat[prei][*j] + mat[*i][posj] - mat[prei][*i] - mat[*j][posj];
    }

    inline double twoopt_cost(TSPSolution *sol, int o, int p) {
        auto i = sol->it(o), j = sol->it(p);
        if (o > p) return twoopt_cost(sol, j, i);
        else       return twoopt_cost(sol, i, j);
    }


    void TwoOptMove::apply(TSPSolution *sol) {
        auto i = sol->it(a), j = sol->it(b);
        twoopt_apply(sol, i, j, twoopt_cost(sol, i, j));
    }

    void TwoOptMove::undo(TSPSolution *sol) {
        apply(sol);
    }

    double TwoOptMove::twoopt_best(TSPSolution *sol, bool auto_push) {
        double delta = INFINITYLF;
        vecit bi, bj;
        auto end = sol->end() - 1, end2 = end - 1;
        if (sol->size() < 5) return delta;
        for (auto i = sol->begin() + 1; i != end2; i++) {
            for (auto j = i + 2; j != end; j++) {
                double d = twoopt_cost(sol, i, j);
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
                int i = std::distance(sol->begin(), bi);
                int j = std::distance(sol->begin(), bj);
                static_cast<StackedTSPSolution*>(sol)->push_NeighborhoodMove(std::unique_ptr<NeighborhoodMove>(new TwoOptMove(i, j)));
            }
        } else vprintf("Positive delta %.0lf => NOP\n", delta);
        return delta;
    }











    inline void reinsertion_apply(TSPSolution *sol, size_t opos, size_t len, size_t npos, double delta) {
        tsp_nmcc_reinsertion++;
        sol->cost += delta;
        //std::cout << "Reinserting [" << opos << "..(" << len << ")] to " << npos << " with delta=" << delta << std::endl;
        std::vector<int> cpy(sol->it(opos), sol->it(opos + len));
        sol->erase(sol->it(opos), sol->it(opos + len));
        size_t dlen = opos < npos ? len : 0;
        sol->insert(sol->it(npos - dlen), cpy.begin(), cpy.end());
    }

    inline double reinsertion_cost(TSPSolution *sol, const vecit &o, size_t len, const vecit &n) {
        const int preo = *(o - 1), poso = *(o + len), pren = *(n - 1), posn = *n,
                bego = *o, endo = *(o + (len - 1));
        return mat[preo][poso] + mat[pren][bego] + mat[endo][posn]
            - (mat[preo][bego] + mat[endo][poso] + mat[pren][posn]);
    }

    inline double reinsertion_cost(TSPSolution *sol, size_t opos, size_t len, size_t npos) {
        auto o = sol->it(opos), n = sol->it(npos);
        return reinsertion_cost(sol, o, len, n);
    }


    void ReinsertionMove::apply(TSPSolution *sol) {
        reinsertion_apply(sol, opos, len, npos, reinsertion_cost(sol, opos, len, npos));
    }

    void ReinsertionMove::undo(TSPSolution *sol) {
        int t = opos;
        opos = npos;
        npos = t;
        if (npos > opos) npos += len;
        else opos -= len;
        //std::cout << "U:npos=" << npos << ", opos=" << opos << " ";
        apply(sol);
    }

    double ReinsertionMove::reinsertion_best(TSPSolution *sol, size_t len, bool auto_push) {
        double delta = INFINITYLF;
        vecit bi, bn;
        auto end_max = sol->end() - len;
        if (sol->size() < len + 3) return delta;
        for (auto i = sol->begin() + 1; i < end_max; i++) {
            auto maxi = i + len;
            for (auto n = sol->begin() + 1; n < sol->end() - 1; n++) {
                if (i <= n && n <= maxi) continue;
                double d = reinsertion_cost(sol, i, len, n);
                if (d < delta) {
                    bi = i;
                    bn = n;
                    delta = d;
                }
            }
        }

        if (delta < 0) {
            vprintf("Passive ReinsertionMove with delta %.0lf\n", delta);
            int o = std::distance(sol->begin(), bi);
            int n = std::distance(sol->begin(), bn);
            if (!auto_push) reinsertion_apply(sol, o, len, n, delta);
            else {
                static_cast<StackedTSPSolution*>(sol)->push_NeighborhoodMove(std::unique_ptr<NeighborhoodMove>(new ReinsertionMove(o, len, n)));
            }
        } else vprintf("Positive delta %.0lf => NOP\n", delta);
        return delta;
    }

}