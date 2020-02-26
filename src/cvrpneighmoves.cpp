#include <algorithm>
#include <assert.h>
#include "cvrpneighmoves.h"


#define mat sol->matrizAdj

#ifdef vprintfen
#define vprintf dprintf
#else
#define vprintf(...) {}
#endif

using namespace std;

namespace CVRPMH {

    typedef TSPMH::TSPSolution TSPSolution;
    typedef TSPMH::vecit vecit;


    inline int getSubRouteIndex(CVRPSolution* sol, int solindex) {
        auto a = upper_bound(sol->subroutes.begin(), sol->subroutes.end(), solindex);
        return a - sol->subroutes.begin() - 1;
    }

    inline int getSubRouteIndex(CVRPSolution* sol, vecit it) {
        return getSubRouteIndex(sol, distance(sol->begin(), it));
    }

    #define __implies(a, b) !(a) || (b)
    inline SubRoute getNearSubRoute(CVRPSolution* sol, int solindex) {
        auto d = getSubRouteIndex(sol, solindex);
        assert(d >= 0);
        assert(__implies(solindex == 0, d == 0));
        assert(__implies(solindex < sol->subroutes[1], d == 0));
        assert(__implies(solindex >= sol->subroutes[1], d > 0));
        return SubRoute::importfrom(sol, d);
    }

    inline SubRoute getNearSubRoute(CVRPSolution* sol, vecit it) {
        return getNearSubRoute(sol, distance(sol->begin(), it));
    }



    const double INFINITYLF = numeric_limits<double>::infinity();

    template<bool setMode>
    inline pair<double, double> swap_dcap(CVRPSolution *sol, const vecit &i, const vecit &j) {
        if (sol->isSubRoute) return make_pair(0, 0);
        else {
            auto ci = getSubRouteIndex(sol, i), cj = getSubRouteIndex(sol, j);
            if (ci == cj) return make_pair(0, 0);
            else {
                auto di = sol->demand[*j]-sol->demand[*i], dj = sol->demand[*i]-sol->demand[*j];
                if constexpr (setMode) {
                    sol->subcapacity[ci] += di;
                    sol->subcapacity[cj] += dj;
                }
                return make_pair(sol->subcapacity[ci] + di, sol->subcapacity[cj] + dj);
            }
        }
    }

    inline void swap_apply(CVRPSolution *sol, vecit &i, vecit &j, double delta) {
        swap_dcap<true>(sol, i, j);
        sol->cost += delta;
        int tmpi = *i;
        *i = *j;
        *j = tmpi;
    }

    inline double swap_cost(CVRPSolution *sol, const vecit &i, const vecit &j) {
        const int prej = *(j - 1), posj = *(j + 1), prei = *(i - 1), posi = *(i + 1);
        if (posi == *j) {
            return mat[prei][*j] + mat[*i][posj] - mat[prei][*i] - mat[*j][posj];
        }
        return mat[prej][*i] + mat[*i][posj] + mat[prei][*j] + mat[*j][posi]
            - (mat[prej][*j] + mat[*j][posj] + mat[prei][*i] + mat[*i][posi]);
    }

    inline double swap_cost(CVRPSolution *sol, int o, int p) {
        auto i = sol->it(o), j = sol->it(p);
        if (o > p) return swap_cost(sol, j, i);
        else       return swap_cost(sol, i, j);
    }



    void SwapMove::apply(TSPSolution *sol) {
        // auto i = sol->it(a), j = sol->it(b);
        // auto s = static_cast<CVRPSolution*>(sol);
        // swap_apply(s, i, j, swap_cost(s, i, j));
    }

    void SwapMove::undo(TSPSolution *sol) {
        // apply(sol);
    }

    MovementResult SwapMove::swap_best(CVRPSolution *sol) {
        double delta = INFINITYLF;
        vecit bi, bj;
        auto end = sol->end() - 1;
        for (auto i = sol->begin() + 1; i != end; i++) {
            for (auto j = i + 1; j != end; j++) {
                double d = swap_cost(sol, i, j);
                if (d < delta && *i != CVRPSolution::route_start && *j != CVRPSolution::route_start) {
                    auto dc = swap_dcap<false>(sol, i, j);
                    if (dc.first > sol->maxcapacity || dc.second > sol->maxcapacity) continue;
                    bi = i;
                    bj = j;
                    delta = d;
                }
            }
        }

        if (delta < 0) {
            vprintf("Passive SwapMove with delta %.0lf\n", delta);
            auto si = getSubRouteIndex(sol, bi), sj = getSubRouteIndex(sol, bj);
            swap_apply(sol, bi, bj, delta);
            return make_pair(SubRoute::importfrom(sol, si), SubRoute::importfrom(sol, sj));
        } else {
            vprintf("Positive delta %.0lf => NOP\n", delta);
            return {};
        }
    }








    inline void reinsertion_apply(CVRPSolution *sol, size_t opos, size_t len, size_t npos, double delta) {
        sol->cost += delta;
        //cout << "Reinserting [" << opos << "..(" << len << ")] to " << npos << " with delta=" << delta << endl;
        vector<int> cpy(sol->it(opos), sol->it(opos + len));
        sol->erase(sol->it(opos), sol->it(opos + len));
        size_t dlen = opos < npos ? len : 0;
        sol->insert(sol->it(npos - dlen), cpy.begin(), cpy.end());
        sol->updateSubRoutes();
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

    //int shitccagain = 0; // TODO: R
    inline pair<double, double> reinsertion_dcap(CVRPSolution *sol, const vecit &o, size_t len, const vecit &n) {
        if (sol->isSubRoute) return make_pair(0, 0);
        else {
            //std::cout << ++shitccagain << std::endl;
            auto co = getSubRouteIndex(sol, o), cn = getSubRouteIndex(sol, n);
            if (co == cn) return make_pair(0, 0);
            else {
                int dcap = 0;
                for (size_t i = 0; i < len; i++) {
                    dcap += sol->demand[*(o+i)];
                }
                return make_pair(sol->subcapacity[co] - dcap, sol->subcapacity[cn] + dcap);
            }
        }
    }


    void ReinsertionMove::apply(TSPSolution *sol) {
        // reinsertion_apply(static_cast<CVRPSolution*>(sol), opos, len, npos, reinsertion_cost(sol, opos, len, npos));
    }

    void ReinsertionMove::undo(TSPSolution *sol) {
        // int t = opos;
        // opos = npos;
        // npos = t;
        // if (npos > opos) npos += len;
        // else opos -= len;
        // //cout << "U:npos=" << npos << ", opos=" << opos << " ";
        // apply(sol);
    }

    MovementResult ReinsertionMove::reinsertion_best(CVRPSolution *sol, size_t len) {
        double delta = INFINITYLF;
        vecit bi, bn;
        auto end_max = sol->end() - len, bg = sol->begin() + 1;
        for (size_t i = 0; i < len; i++) {
            if (*(bg+i) == CVRPSolution::route_start) {
                bg = bg+i+1;
                i = 0;
            }
        }
        size_t icc=0, jcc, szd = sol->size(); // TODO: R
        for (auto i = bg; i < end_max; i++) {
            auto maxi = i + len;
            if (*(maxi-1) == CVRPSolution::route_start) {
                i += (len - 1);
                continue;
            }
            icc = distance(sol->begin(), i);
            //if (*(i-1) == CVRPSolution::route_start || *maxi == CVRPSolution::route_start) continue;
            for (auto n = sol->begin() + 1; n != sol->end() - 1; n++) {
                jcc = distance(sol->begin(), n);
                if (i <= n && n <= maxi) continue;
                if (*n == CVRPSolution::route_start && *(n-1) != CVRPSolution::route_start) continue;
                double d = reinsertion_cost(sol, i, len, n);
                if (d < delta && reinsertion_dcap(sol, i, len, n).second <= sol->maxcapacity) {
                    bi = i;
                    bn = n;
                    delta = d;
                }
            }
        }

        if (delta < 0) {
            int o = distance(sol->begin(), bi), n = distance(sol->begin(), bn);
            auto so = getSubRouteIndex(sol, o), sn = getSubRouteIndex(sol, n);
            vprintf("Passive ReinsertionMove with delta %.0lf; origin: %d {len: %lud}; dest: %d\n", delta, o, len, n);
            reinsertion_apply(sol, o, len, n, delta);
            return make_pair(SubRoute::importfrom(sol, so), SubRoute::importfrom(sol, sn));
        } else {
            vprintf("Positive delta %.0lf => NOP\n", delta);
            return {};
        }
    }

}