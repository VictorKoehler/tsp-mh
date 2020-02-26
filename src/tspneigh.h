#ifndef __TSPNEIGHBORHOOD_CLASS__
#define __TSPNEIGHBORHOOD_CLASS__

#define PRINTDEBUG 1

#ifndef NOTDEBUG
#define PRINTDEBUG 1
#define ifdebug(...) __VA_ARGS__
#else
#define ifdebug(...)
#endif

#ifdef PRINTDEBUG
#define dprintf(...) printf(__VA_ARGS__)
#else
#define dprintf(...)
#endif


namespace TSPMH {

    class TSPSolution;
    class NeighborhoodMove {
        public:
        virtual void apply(TSPSolution *sol) {};
        virtual void undo(TSPSolution *sol) {};
        virtual void best(TSPSolution *sol, bool auto_push) {};
        virtual ~NeighborhoodMove() {}
    };
    
}

#endif
