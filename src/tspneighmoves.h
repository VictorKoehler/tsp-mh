#ifndef __TSPNEIGHMOVES__
#define __TSPNEIGHMOVES__
#include "tspsolution.h"


namespace TSPMH {

    template <typename U>
    class SwapMove : public NeighborhoodMove<U> {
        public:
        int a, b;
        
        SwapMove() {}
        SwapMove(const int i, const int j) : a(i < j ? i : j), b(i < j ? j : i) { }

        void apply(TSPSolution<U> *sol) override;
        void undo(TSPSolution<U> *sol) override;
        void best(TSPSolution<U> *sol, bool auto_push) override {
            swap_best(sol, auto_push);
        };

        ~SwapMove() override {}

        static U swap_best(TSPSolution<U> *sol, bool auto_push);
    };


    template <typename U>
    class TwoOptMove : public NeighborhoodMove<U> {
        public:
        int a, b;
        
        TwoOptMove() {}
        TwoOptMove(const int i, const int j) : a(i < j ? i : j), b(i < j ? j : i) { }

        void apply(TSPSolution<U> *sol) override;
        void undo(TSPSolution<U> *sol) override;
        void best(TSPSolution<U> *sol, bool auto_push) override {
            twoopt_best(sol, auto_push);
        };

        ~TwoOptMove() override {}

        static U twoopt_best(TSPSolution<U> *sol, bool auto_push);
    };


    template <typename U>
    class ReinsertionMove : public NeighborhoodMove<U> {
        public:
        int opos, len, npos;
        
        ReinsertionMove(const int len) : len(len) {}
        ReinsertionMove(const int opos, const int len, const int npos) : opos(opos), len(len), npos(npos) { }

        void apply(TSPSolution<U> *sol) override;
        void undo(TSPSolution<U> *sol) override;
        void best(TSPSolution<U> *sol, bool auto_push) override {
            reinsertion_best(sol, len, auto_push);
        };

        ~ReinsertionMove() override {}

        static U reinsertion_best(TSPSolution<U> *sol, size_t len, bool auto_push);
    };

    template class SwapMove<int>;
    template class SwapMove<double>;
    template class TwoOptMove<int>;
    template class TwoOptMove<double>;
    template class ReinsertionMove<int>;
    template class ReinsertionMove<double>;

}
#endif