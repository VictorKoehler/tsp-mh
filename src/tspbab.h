#include "bab.h"

namespace TSPBaB {
    
    struct TSPNode : BranchAndBound::Node {
        public:
        std::vector< std::pair<int, int> > arcs;
        std::vector<int> subtour;

        TSPNode(int lb) {
            lower_bound = lb;
        }

        void solve(BranchAndBound::Tree* tree) override;
        ~TSPNode() override {}
    };

    std::shared_ptr<TSPNode> solveTSPBab(int dimension, double **matrix, int upper_bound=BranchAndBound::INT_HIGH);
}