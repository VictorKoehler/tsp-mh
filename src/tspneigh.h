#ifndef __TSPNEIGHBORHOOD_CLASS__
#define __TSPNEIGHBORHOOD_CLASS__


#ifndef NDEBUG
#define dprintf(...) printf(__VA_ARGS__)
#else
#define dprintf(...)
#endif


class TSPSolution;
class NeighborhoodMove {
    public:
    virtual void apply(TSPSolution *sol) {};
    virtual void undo(TSPSolution *sol) {};
    virtual void best(TSPSolution *sol, bool auto_push) {};
    virtual ~NeighborhoodMove() {}
};

#endif