#ifndef __TSPNEIGHBORHOOD_CLASS__
#define __TSPNEIGHBORHOOD_CLASS__


#ifndef NDEBUG
#define dprintf(...) printf(__VA_ARGS__)
#else
#define dprintf(...)
#endif


namespace TSPMH {

    template <typename U>
    class TSPSolution;

    template <typename U>
    class NeighborhoodMove {
        public:
        virtual void apply(TSPSolution<U> *sol) {};
        virtual void undo(TSPSolution<U> *sol) {};
        virtual void best(TSPSolution<U> *sol, bool auto_push) {};
        virtual ~NeighborhoodMove() {}
    };
    
    template class NeighborhoodMove<int>;
    template class NeighborhoodMove<double>;
}

#endif