#include <algorithm>
#include <assert.h>
#include "tspneighmoves.h"

const double INFINITYLF = std::numeric_limits<double>::infinity();

#define mat sol->matrizAdj
#define it(i) begin() + i

#ifdef vprintfen
#define vprintf dprintf
#else
#define vprintf(...)
#endif


inline void swap_apply(MLPSolution *sol, vecit &i, vecit &j, double ncost, double ndur) {
    sol->cost = ncost;
    sol->duration = ndur;
    int tmpi = *i;
    *i = *j;
    *j = tmpi;
}



#define add(i) _add(i, mat, sol)

inline ConcatenationInterv swap_cost(MLPSolution *sol, int i, int j) {
    assert(i < j);
    const auto last = sol->dimension;
    const auto subseq = sol->subseqConcatenation;
    if (i + 1 == j) {
        // [0..*] [1] [2] [3..*]
        // [0..*] [2] [1] [3..*]
        auto seg0 = subcint(0, i-1), seg1 = subcint(i, i), seg2 = subcint(j, j), seg3 = subcint(j+1, last);
        auto d = seg0.add(seg2).add(seg1).add(seg3);
        return d;
    } else {
        // [0..*] [1] [2..*] [3] [4..*]
        // [0..*] [3] [2..*] [1] [4..*]
        auto seg0 = subcint(0, i-1), seg1 = subcint(i, i), seg2 = subcint(i+1, j-1), seg3 = subcint(j, j), seg4 = subcint(j+1, last);
        auto d = seg0.add(seg3).add(seg2).add(seg1).add(seg4);
        return d;
    }
}



void SwapMove::apply(MLPSolution *sol) {
    auto i = sol->it(a), j = sol->it(b);
    auto v = swap_cost(sol, a, b);
    swap_apply(sol, i, j, v.c, v.t);
}

void SwapMove::undo(MLPSolution *sol) {
    apply(sol);
}

double SwapMove::swap_best(MLPSolution *sol, bool auto_push) {
    double ncost = sol->cost, ndur = sol->duration;
    size_t bi, bj;
    auto end = sol->size() - 1;
    for (size_t i = 1; i != end; i++) {
        for (size_t j = i + 1; j != end; j++) {
            auto d0 = swap_cost(sol, i, j);
            if (d0.c < ncost) {
                bi = i;
                bj = j;
                ncost = d0.c;
                ndur = d0.t;
            }
        }
    }

    if (ncost < sol->cost) {
        vprintf("Passive SwapMove with delta %.0lf\n", delta);
        auto i = sol->it(bi), j = sol->it(bj);
        if (!auto_push) swap_apply(sol, i, j, ncost, ndur);
        else {
            sol->push_NeighborhoodMove(std::unique_ptr<NeighborhoodMove>(new SwapMove(bi, bj)));
        }
    } else vprintf("Positive delta %.0lf => NOP\n", delta);
    return ncost;
}










inline void twoopt_apply(MLPSolution *sol, vecit &i, vecit &j, double delta) {
    sol->cost += delta;
    std::reverse(i, j+1);
}

inline double twoopt_cost(MLPSolution *sol, const vecit &i, const vecit &j) {
    const int prej = *(j - 1), posj = *(j + 1), prei = *(i - 1), posi = *(i + 1);
    assert(posi != *j);
    assert(mat[prei][*j] == mat[*j][prei] && mat[*j][posj] == mat[posj][*j]);
    assert(mat[prej][*i] == mat[*i][prej] && mat[*i][posi] == mat[posi][*i]);
    return mat[prei][*j] + mat[*i][posj] - mat[prei][*i] - mat[*j][posj];
}

inline double twoopt_cost(MLPSolution *sol, int o, int p) {
    auto i = sol->it(o), j = sol->it(p);
    if (o > p) return twoopt_cost(sol, j, i);
    else       return twoopt_cost(sol, i, j);
}


void TwoOptMove::apply(MLPSolution *sol) {
    auto i = sol->it(a), j = sol->it(b);
    twoopt_apply(sol, i, j, twoopt_cost(sol, i, j));
}

void TwoOptMove::undo(MLPSolution *sol) {
    apply(sol);
}

double TwoOptMove::twoopt_best(MLPSolution *sol, bool auto_push) {
    double delta = INFINITYLF;
    vecit bi, bj;
    auto end = sol->end() - 1, end2 = end - 1;
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
            sol->push_NeighborhoodMove(std::unique_ptr<NeighborhoodMove>(new TwoOptMove(i, j)));
        }
    } else vprintf("Positive delta %.0lf => NOP\n", delta);
    return delta;
}











inline void reinsertion_apply(MLPSolution *sol, size_t opos, size_t len, size_t npos, double delta) {
    sol->cost += delta;
    //std::cout << "Reinserting [" << opos << "..(" << len << ")] to " << npos << " with delta=" << delta << std::endl;
    std::vector<int> cpy(sol->it(opos), sol->it(opos + len));
    sol->erase(sol->it(opos), sol->it(opos + len));
    size_t dlen = opos < npos ? len : 0;
    sol->insert(sol->it(npos - dlen), cpy.begin(), cpy.end());
}

inline double reinsertion_cost(MLPSolution *sol, const vecit &o, size_t len, const vecit &n) {
    const int preo = *(o - 1), poso = *(o + len), pren = *(n - 1), posn = *n,
              bego = *o, endo = *(o + (len - 1));
    return mat[preo][poso] + mat[pren][bego] + mat[endo][posn]
        - (mat[preo][bego] + mat[endo][poso] + mat[pren][posn]);
}

inline double reinsertion_cost(MLPSolution *sol, size_t opos, size_t len, size_t npos) {
    auto o = sol->it(opos), n = sol->it(npos);
    return reinsertion_cost(sol, o, len, n);
}


void ReinsertionMove::apply(MLPSolution *sol) {
    reinsertion_apply(sol, opos, len, npos, reinsertion_cost(sol, opos, len, npos));
}

void ReinsertionMove::undo(MLPSolution *sol) {
    int t = opos;
    opos = npos;
    npos = t;
    if (npos > opos) npos += len;
    else opos -= len;
    //std::cout << "U:npos=" << npos << ", opos=" << opos << " ";
    apply(sol);
}

double ReinsertionMove::reinsertion_best(MLPSolution *sol, size_t len, bool auto_push) {
    double delta = INFINITYLF;
    vecit bi, bn;
    auto end_max = sol->end() - len;
    for (auto i = sol->begin() + 1; i != end_max; i++) {
        auto maxi = i + len;
        for (auto n = sol->begin() + 1; n != sol->end() - 1; n++) {
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
            sol->push_NeighborhoodMove(std::unique_ptr<NeighborhoodMove>(new ReinsertionMove(o, len, n)));
        }
    } else vprintf("Positive delta %.0lf => NOP\n", delta);
    return delta;
}
