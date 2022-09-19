#ifndef __TSPNEIGHBORHOOD_CLASS__
#define __TSPNEIGHBORHOOD_CLASS__


#ifndef NDEBUG
#define dprintf(...) printf(__VA_ARGS__)
#else
#define dprintf(...)
#endif


namespace TSPMH {

    class StackedTSPSolution;
    class NeighborhoodMove {
        public:
        virtual void apply(StackedTSPSolution *sol) {};
        virtual void undo(StackedTSPSolution *sol) {};
        virtual void best(StackedTSPSolution *sol, bool auto_push) {};
        virtual ~NeighborhoodMove() {}
    };
    
}

#endif