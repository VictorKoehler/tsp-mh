#ifndef __CVRPNEIGHMOVES__
#define __CVRPNEIGHMOVES__
#include "cvrpsolution.h"
#include <optional>

namespace CVRPMH {

    typedef std::optional<std::pair<SubRoute, SubRoute>> MovementResult;

    class SwapMove : public TSPMH::NeighborhoodMove {
        public:
        int a, b;
        
        SwapMove() {}
        SwapMove(const int i, const int j) : a(i < j ? i : j), b(i < j ? j : i) { }

        void apply(TSPMH::TSPSolution *sol) override;
        void undo(TSPMH::TSPSolution *sol) override;
        void best(TSPMH::TSPSolution *sol, bool auto_push) override {
            swap_best(static_cast<CVRPSolution*>(sol));
        };

        ~SwapMove() override {}

        static MovementResult swap_best(CVRPSolution *sol);
    };


    class ReinsertionMove : public TSPMH::NeighborhoodMove {
        public:
        int opos, len, npos;
        
        ReinsertionMove(const int len) : len(len) {}
        ReinsertionMove(const int opos, const int len, const int npos) : opos(opos), len(len), npos(npos) { }

        void apply(TSPMH::TSPSolution *sol) override;
        void undo(TSPMH::TSPSolution *sol) override;
        void best(TSPMH::TSPSolution *sol, bool auto_push) override {
            reinsertion_best(static_cast<CVRPSolution*>(sol), len);
        };

        ~ReinsertionMove() override {}

        static MovementResult reinsertion_best(CVRPSolution *sol, size_t len);
    };

}
#endif