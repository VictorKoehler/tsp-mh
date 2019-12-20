#ifndef __TSPNEIGHMOVES__
#define __TSPNEIGHMOVES__
#include "tspsolution.h"

class SwapMove : public NeighborhoodMove {
    public:
    int a, b;
    
    SwapMove() {}
    SwapMove(const int i, const int j) : a(i < j ? i : j), b(i < j ? j : i) { }

    void apply(MLPSolution *sol) override;
    void undo(MLPSolution *sol) override;
    void best(MLPSolution *sol, bool auto_push) override {
        swap_best(sol, auto_push);
    };

    ~SwapMove() override {}

    static double swap_best(MLPSolution *sol, bool auto_push);
};


class TwoOptMove : public NeighborhoodMove {
    public:
    int a, b;
    
    TwoOptMove() {}
    TwoOptMove(const int i, const int j) : a(i < j ? i : j), b(i < j ? j : i) { }

    void apply(MLPSolution *sol) override;
    void undo(MLPSolution *sol) override;
    void best(MLPSolution *sol, bool auto_push) override {
        twoopt_best(sol, auto_push);
    };

    ~TwoOptMove() override {}

    static double twoopt_best(MLPSolution *sol, bool auto_push);
};


class ReinsertionMove : public NeighborhoodMove {
    public:
    int opos, len, npos;
    
    ReinsertionMove(const int len) : len(len) {}
    ReinsertionMove(const int opos, const int len, const int npos) : opos(opos), len(len), npos(npos) { }

    void apply(MLPSolution *sol) override;
    void undo(MLPSolution *sol) override;
    void best(MLPSolution *sol, bool auto_push) override {
        reinsertion_best(sol, len, auto_push);
    };

    ~ReinsertionMove() override {}

    static double reinsertion_best(MLPSolution *sol, size_t len, bool auto_push);
};

#endif