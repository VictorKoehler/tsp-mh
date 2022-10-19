#include "bab.h"
#include "lagrangian.h"

namespace TSPBaB {
    class TSPLagrangianBaBTree : public BranchAndBound::Tree {
        const Data& data;
        public:
        TSPLagrangianBaBTree(const Data& data, double upper=std::numeric_limits<double>::max());
        void solve();
    };

    class TSPLagrNode : public BranchAndBound::Node {
        friend TSPLagrangianBaBTree;
        TSPLagrangian::LagrangianTSP lagr;
        
        TSPLagrNode(const TSPLagrangian::LagrangianTSP& l) : lagr(l) {}
        TSPLagrNode(const Data& data) : lagr(data) {}

        public:
        bool solve(BranchAndBound::Tree* tree);
        virtual double getLowerBound() const override { return lagr.getSolutionLB(); }
        const TSPLagrangian::LagrangianTSP& getSolution() const { return lagr; };
        ~TSPLagrNode() override { }
    };
}
