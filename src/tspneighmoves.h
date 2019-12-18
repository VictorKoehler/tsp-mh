#include "tspsolution.h"

class SwapMove : public NeighborhoodMove {
    public:
    int a, b;
    
    SwapMove(const int i, const int j) : a(i), b(j) { }

    void apply(TSPSolution *sol) override;
    void undo(TSPSolution *sol) override;

    ~SwapMove() override {}

    inline static double swap_cost(TSPSolution *sol, int o, int p);
};