#include <vector>
#include <queue>
#include <iostream>
#include <memory>

namespace BranchAndBound {
    
    const uint route_start = 0;

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
        protected:
        int upper_bound;
        std::priority_queue< NodePtr, std::vector<NodePtr>, NodeComparator > nodes;
        NodePtr best_node;

        public:
        Tree(int upper=std::numeric_limits<int>::max()) : upper_bound(upper) { }
        virtual ~Tree() {}

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

        const NodePtr& getBest() const {
            return best_node;
        }
    };
}