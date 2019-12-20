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
        if (!auto_push) {
            auto i = sol->it(bi), j = sol->it(bj);
            swap_apply(sol, i, j, ncost, ndur);
        }
        else {
            sol->push_NeighborhoodMove(std::unique_ptr<NeighborhoodMove>(new SwapMove(bi, bj)));
        }
    } else vprintf("Positive delta %.0lf => NOP\n", delta);
    return ncost;
}










inline void twoopt_apply(MLPSolution *sol, vecit &i, vecit &j, double ncost, double ndur) {
    sol->cost = ncost;
    sol->duration = ndur;
    std::reverse(i, j+1);
}


inline ConcatenationInterv twoopt_cost(MLPSolution *sol, int i, int j) {
    assert(i < j);
    const auto last = sol->dimension;
    const auto subseq = sol->subseqConcatenation;
    // [0..*] [1..*] [2..*]
    // [0..*] [*..1] [2..*]
    auto seg0 = subcint(0, i-1), ges1 = subcint(j, i), seg2 = subcint(j+1, last);
    auto d = seg0.add(ges1).add(seg2);
    return d;
}


void TwoOptMove::apply(MLPSolution *sol) {
    auto i = sol->it(a), j = sol->it(b);
    auto v = twoopt_cost(sol, a, b);
    twoopt_apply(sol, i, j, v.c, v.t);
}

void TwoOptMove::undo(MLPSolution *sol) {
    apply(sol);
}

double TwoOptMove::twoopt_best(MLPSolution *sol, bool auto_push) {
    double ncost = sol->cost, ndur = sol->duration;
    size_t bi, bj;
    size_t end = sol->size() - 1, end2 = end - 1;
    for (size_t i = 1; i != end2; i++) {
        for (auto j = i + 2; j != end; j++) {
            auto d0 = twoopt_cost(sol, i, j);
            if (d0.c < ncost) {
                bi = i;
                bj = j;
                ncost = d0.c;
                ndur = d0.t;
            }
        }
    }

    if (ncost < sol->cost) {
        vprintf("Passive TwoOptMove with delta %.0lf\n", delta);
        if (!auto_push) {
            auto i = sol->it(bi), j = sol->it(bj);
            twoopt_apply(sol, i, j, ncost, ndur);
        }
        else {
            sol->push_NeighborhoodMove(std::unique_ptr<NeighborhoodMove>(new TwoOptMove(bi, bj)));
        }
    } else vprintf("Positive delta %.0lf => NOP\n", delta);
    return ncost;
}











inline void reinsertion_apply(MLPSolution *sol, size_t opos, size_t len, size_t npos, double ncost, double ndur) {
    sol->cost = ncost;
    sol->duration = ndur;
    //std::cout << "Reinserting [" << opos << "..(" << len << ")] to " << npos << " with delta=" << delta << std::endl;
    std::vector<int> cpy(sol->it(opos), sol->it(opos + len));
    sol->erase(sol->it(opos), sol->it(opos + len));
    size_t dlen = opos < npos ? len : 0;
    sol->insert(sol->it(npos - dlen), cpy.begin(), cpy.end());
}

inline ConcatenationInterv reinsertion_cost(MLPSolution *sol, size_t i, size_t len, size_t npos) {
    size_t j = i + len - 1; // it is inclusive
    assert(npos < i || npos > j);
    const auto last = sol->dimension;
    const auto subseq = sol->subseqConcatenation;
    if (npos < i) {
        // [0..*] ![1..*] <2..*> [3..*]
        // [0..*]  <2..*> [1..*] [3..*]
        auto seg0 = subcint(0, npos-1), seg1 = subcint(npos, i-1), seg2 = subcint(i, j), seg3 = subcint(j+1, last);
        auto d = seg0.add(seg2).add(seg1).add(seg3);
        return d;
    } else {
        // [0..*] <1..*> [2..*] ![3..*]
        // [0..*] [2..*] <1..*>  [3..*]
        auto seg0 = subcint(0, i-1), seg1 = subcint(i, j), seg2 = subcint(j+1, npos-1), seg3 = subcint(npos, last);
        auto d = seg0.add(seg2).add(seg1).add(seg3);
        return d;
    }
}


void ReinsertionMove::apply(MLPSolution *sol) {
    auto v = reinsertion_cost(sol, opos, len, npos);
    reinsertion_apply(sol, opos, len, npos, v.c, v.t);
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
    double ncost = sol->cost, ndur = sol->duration;
    size_t bi, bn;
    size_t sz = sol->size(), end_max = sz - len;
    for (size_t i = 1; i != end_max; i++) {
        size_t maxi = i + len;
        for (size_t n = 1; n != sz - 1; n++) {
            if (i <= n && n <= maxi) continue;
            auto d0 = reinsertion_cost(sol, i, len, n);
            if (d0.c < ncost) {
                bi = i;
                bn = n;
                ncost = d0.c;
                ndur = d0.t;
            }
        }
    }

    if (ncost < sol->cost) {
        vprintf("Passive ReinsertionMove with delta %.0lf\n", delta);
        if (!auto_push) {
            reinsertion_apply(sol, bi, len, bn, ncost, ndur);
        }
        else {
            sol->push_NeighborhoodMove(std::unique_ptr<NeighborhoodMove>(new ReinsertionMove(bi, len, bn)));
        }
    } else vprintf("Positive delta %.0lf => NOP\n", delta);
    return ncost;
}
