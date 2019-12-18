#include "tspneighmoves.h"

#define mat sol->matrizAdj
#define it(i) begin() + i

void SwapMove::apply(TSPSolution *sol) {
    sol->cost += swap_cost(sol, a, b);
    auto i = sol->it(a), j = sol->it(b);
    int tmpi = *i;
    *i = *j;
    *j = tmpi;
}

void SwapMove::undo(TSPSolution *sol) {
    apply(sol);
}

inline double SwapMove::swap_cost(TSPSolution *sol, int o, int p)
{
    if (o > p) return swap_cost(sol, p, o);

    auto i = sol->it(o), j = sol->it(p);
    const int prej = *(j - 1), posj = *(j + 1), prei = *(i - 1), posi = *(i + 1);
    if (o + 1 == p) {
        return mat[prei][*j] + mat[*i][posj] - mat[prei][*i] - mat[*j][posj];
    }
    return mat[prej][*i] + mat[*i][posj] + mat[prei][*j] + mat[*j][posi]
        - (mat[prej][*j] + mat[*j][posj] + mat[prei][*i] + mat[*i][posi]);
}