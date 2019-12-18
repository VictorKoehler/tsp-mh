#ifndef __TSPNEIGHMOVES__
#define __TSPNEIGHMOVES__
#include "tspsolution.h"

class SwapMove : public NeighborhoodMove {
    public:
    int a, b;
    
    SwapMove(const int i, const int j) : a(i < j ? i : j), b(i < j ? j : i) { }

    void apply(TSPSolution *sol) override;
    void undo(TSPSolution *sol) override;

    ~SwapMove() override {}

    static double swap_best(TSPSolution *sol, bool auto_push);
};


class TwoOptMove : public NeighborhoodMove {
    public:
    int a, b;
    
    TwoOptMove(const int i, const int j) : a(i < j ? i : j), b(i < j ? j : i) { }

    void apply(TSPSolution *sol) override;
    void undo(TSPSolution *sol) override;

    ~TwoOptMove() override {}

    static double twoopt_best(TSPSolution *sol, bool auto_push);
};

#endif