#include "bab.h"
#include "hungarian.h"

namespace TSPBaB {

    // I didn't test, but I'm scared with the possibility of the Hungarian Algorithm doesn't properly deal with integer overflow.
    const int INT_HIGH = 99999999;
    struct TSPNode;

    class HungarianBaBTree : public BranchAndBound::Tree {
        friend TSPNode;
        uint dimension;
        hungarian_problem_t hp;
        int **costMatrix, **assignmentMatrix;

        TSPNode* hung_subt_dec();

        public:
        HungarianBaBTree(int d, double **m, int upper=INT_HIGH);
        virtual ~HungarianBaBTree();
    };
    
    struct TSPNode : BranchAndBound::Node {
        std::vector< std::pair<int, int> > arcs;
        std::vector<int> subtour;

        TSPNode(int lb) {
            lower_bound = lb;
        }

        void solve(BranchAndBound::Tree* tree) override { solve(dynamic_cast<HungarianBaBTree*>(tree)); };
        void solve(HungarianBaBTree* tree);
        ~TSPNode() override {}

        static std::shared_ptr<TSPNode> solveTSPBab(int dimension, double **matrix, int upper_bound=INT_HIGH);
    };

    std::shared_ptr<TSPNode> solveTSPBab(int dimension, double **matrix, int upper_bound=INT_HIGH);
}
