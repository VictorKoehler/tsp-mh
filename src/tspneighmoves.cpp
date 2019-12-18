#include <algorithm>
#include <assert.h>
#include "tspneighmoves.h"

const double INFINITYLF = std::numeric_limits<double>::infinity();

#define mat sol->matrizAdj
#define it(i) begin() + i


inline void swap_apply(TSPSolution *sol, vecit &i, vecit &j, double delta) {
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
        if (!auto_push) swap_apply(sol, bi, bj, delta);
        else {
            int i = std::distance(sol->begin(), bi);
            int j = std::distance(sol->begin(), bj);
            sol->push_NeighborhoodMove(std::unique_ptr<NeighborhoodMove>(new SwapMove(i, j)));
        }
    }
    return delta;
}










inline void twoopt_apply(TSPSolution *sol, vecit &i, vecit &j, double delta) {
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
        if (!auto_push) twoopt_apply(sol, bi, bj, delta);
        else {
            int i = std::distance(sol->begin(), bi);
            int j = std::distance(sol->begin(), bj);
            sol->push_NeighborhoodMove(std::unique_ptr<NeighborhoodMove>(new TwoOptMove(i, j)));
        }
    }
    return delta;
}
