#include <vector>
#include <queue>
#include <iostream>
#include <numeric>
#include <memory>
#include "hungarian.h"

namespace BranchAndBound {
    
    const uint route_start = 0;

    // I didn't test, but I'm scared with the possibility of the Hungarian Algorithm doesn't properly deal with integer overflow.
    const int INT_HIGH = 99999999;

    class Tree;

    struct Node {
        public:
        Node(const Node&) = delete;
        void operator=(const Node&) = delete;
        Node() {}
        Node(int lb) : lower_bound(lb) {}

        int lower_bound;
        virtual void solve(Tree* tree) = 0; 
        virtual ~Node() {}
    };
    
    typedef std::shared_ptr<Node> NodePtr;

    class NodeComparator {
        public:
        bool operator()(const NodePtr a, const NodePtr b)
        {
            return (a->lower_bound > b->lower_bound);
        }
    };



    #define __copymatrix(src, dst, dimx, dimy) \
    for (uint i = 0; i < dimx; i++) { \
        for (uint j = 0; j < dimy; j++) { \
            dst[i][j] = src[i][j]; \
        } \
    }

    class Tree {
        public:
        uint dimension;
        int upper_bound;
        std::priority_queue< NodePtr, std::vector<NodePtr>, NodeComparator > nodes;
        NodePtr best_node;
        hungarian_problem_t hp;
        int **costMatrix, **assignmentMatrix;


        Tree(int d, double **m, int upper=INT_HIGH) : dimension(d), upper_bound(upper) {
            hungarian_init(&hp, m, d, d, HUNGARIAN_MODE_MINIMIZE_COST);
            costMatrix = hp.cost;
            assignmentMatrix = hp.assignment;
        }

        void solve(NodePtr root) {
            setBest(root, false);
            nodes.push(std::move(root));

            while (!nodes.empty()) {
                NodePtr n = std::move(nodes.top());
                nodes.pop();
                if (n->lower_bound <= upper_bound) {
                    n->solve(this);
                }
            }
        }

        inline void branch(NodePtr node) {
            nodes.push(node);
        }

        inline void branch(Node* node) {
            branch(NodePtr(node));
        }

        inline void setBest(NodePtr node, bool setUB) {
            best_node = node;
            if (setUB) {
                upper_bound = best_node->lower_bound;
            }
        }

        inline void setBest(Node* node, bool setUB) {
            setBest(NodePtr(node), setUB);
        }

        inline double solve_hungarian_problem() {
            int bkpMatrix[dimension][dimension];
            __copymatrix(hp.cost, bkpMatrix, dimension, dimension);
            double d = hungarian_solve(&hp);
            __copymatrix(bkpMatrix, hp.cost, dimension, dimension);
            return d;
        }

        ~Tree() {
            costMatrix = NULL;
            assignmentMatrix = NULL;
            hungarian_free(&hp);
        }
    };

    #undef __copymatrix
}