#ifndef __TSPBAB_CLASS__
#define __TSPBAB_CLASS__

#include <vector>
#include <queue>
#include <iostream>
#include <memory>

namespace BranchAndBound {
    
    const uint route_start = 0;

    class Tree;

    struct Node {
        Node(const Node&) = delete;
        void operator=(const Node&) = delete;
        Node() {}

        virtual double getLowerBound() const = 0;
        inline double lb() const { return getLowerBound(); }
        virtual bool solve(Tree* tree) = 0; 
        virtual ~Node() {}
    };
    
    typedef std::shared_ptr<Node> NodePtr;

    class NodeComparator {
        public:
        bool operator()(const NodePtr a, const NodePtr b) {
            return a->lb() > b->lb();
        }
    };



    class Tree {
        protected:
        double upper_bound;
        std::priority_queue< NodePtr, std::vector<NodePtr>, NodeComparator > nodes;
        NodePtr best_node;

        public:
        Tree(double upper=std::numeric_limits<double>::max()) : upper_bound(upper) { }
        virtual ~Tree() {}

        void solve(NodePtr root) {
            setBest(root, false);
            nodes.push(std::move(root));

            while (!nodes.empty()) {
                NodePtr n = std::move(nodes.top());
                nodes.pop();
                // if (nodes.size() % 1000 == 0) std::cout << "B&B Tree: " << nodes.size() << " nodes, lb=" << n->lb() << ", ub=" << upper_bound << "\n";
                if (n->lb() <= upper_bound + 0.1) {
                    if (n->solve(this)) {
                        std::cout << "Found feasible solution of value " << n->lb() << ", best UB so far was " << upper_bound;
                        if (n->lb() < upper_bound) {
                            std::cout << ". Updating it.\n";
                            setBest(n, true);
                        } else std::cout << ".\n";
                    }
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
                upper_bound = best_node->lb();
            }
        }

        inline void setBest(Node* node, bool setUB) {
            setBest(NodePtr(node), setUB);
        }

        const NodePtr& getBest() const {
            return best_node;
        }

        double getUpperBound() const {
            return upper_bound;
        }

        double getLowerBound() const {
            return getBest() ? getBest()->lb() : -std::numeric_limits<double>::infinity();
        }
    };
}
#endif
