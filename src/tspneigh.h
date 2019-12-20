#ifndef __TSPNEIGHBORHOOD_CLASS__
#define __TSPNEIGHBORHOOD_CLASS__


#ifndef NDEBUG
#define dprintf(...) printf(__VA_ARGS__)
#else
#define dprintf(...)
#endif


class MLPSolution;
class NeighborhoodMove {
    public:
    virtual void apply(MLPSolution *sol) {};
    virtual void undo(MLPSolution *sol) {};
    virtual void best(MLPSolution *sol, bool auto_push) {};
    virtual ~NeighborhoodMove() {}
};

#endif