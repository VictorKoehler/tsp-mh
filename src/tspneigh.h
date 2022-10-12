#ifndef __TSPNEIGHBORHOOD_CLASS__
#define __TSPNEIGHBORHOOD_CLASS__

namespace TSPMH {

    class StackedTSPSolution;
    class NeighborhoodMove {
        public:
        virtual void apply(StackedTSPSolution *) {};
        virtual void undo(StackedTSPSolution *) {};
        virtual void best(StackedTSPSolution *, [[maybe_unused]] bool auto_push) {};
        virtual ~NeighborhoodMove() {}
    };
    
}

#endif