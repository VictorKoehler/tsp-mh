#ifndef __TSPNEIGHBORHOOD_CLASS__
#define __TSPNEIGHBORHOOD_CLASS__

class TSPSolution;
class NeighborhoodMove {
    public:
    virtual void apply(TSPSolution *sol) {};
    virtual void undo(TSPSolution *sol) {};
    virtual ~NeighborhoodMove() {}
};

#endif