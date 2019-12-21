#include <vector>
#include <queue>
#include <iostream>
#include <numeric>
#include <memory>
#include "hungarian.h"

namespace BranchAndBound {
    
    const uint route_start = 0;
    const double INFINITYLF = std::numeric_limits<double>::infinity();

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



    class Tree {
        public:
        uint dimension;
        double **matrizAdj, upper_bound;
        std::priority_queue< NodePtr, std::vector<NodePtr>, NodeComparator > nodes;
        NodePtr best_node;

        Tree(uint d, double **m, NodePtr root, double upper=INFINITYLF) : dimension(d), matrizAdj(m), upper_bound(upper) {
            nodes.push(std::move(root));
        }

        void solve() {
            while (!nodes.empty()) {
                NodePtr n = std::move(nodes.top());
                nodes.pop();
                if (n->lower_bound <= upper_bound) {
                    n->solve(this);
                }
            }
        }

        inline void branch(Node* node) {
            nodes.push(shared_ptr<Node>(node));
        }
    };
}