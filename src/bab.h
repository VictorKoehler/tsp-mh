#ifndef __TSPBAB_CLASS__
#define __TSPBAB_CLASS__

#include <vector>
#include <queue>
#include <memory>
#include <limits>
#include "global.h"

namespace BranchAndBound {
    
    const uint route_start = 0;

    class Tree;

    struct Node {
        Node(const Node&) = delete;
        void operator=(const Node&) = delete;
        Node() {}

        inline double lb() const { return getLowerBound(); }
        virtual double getLowerBound() const = 0;
        virtual bool solve(Tree* tree) = 0; 
        virtual ~Node() {}
    };
    
    typedef std::shared_ptr<Node> NodePtr;

    class NodeComparator {
        public:
        bool operator()(const NodePtr a, const NodePtr b) { return a->lb() > b->lb(); }
    };



    class Tree {
        protected:
        typedef std::priority_queue< NodePtr, std::vector<NodePtr>, NodeComparator > pqueue;
        double upper_bound;
        pqueue nodes;
        NodePtr best_node;
        int time_limit;

        public:
        Tree(double upper=std::numeric_limits<double>::max(), int time_limit=5) : upper_bound(upper), time_limit(time_limit) { }
        virtual ~Tree() {}
        void solve(NodePtr root);
        void branch(NodePtr node);
        void branch(Node* node);
        void setBest(NodePtr node, bool setUB);
        void setBest(Node* node, bool setUB);
        const NodePtr& getBest() const;
        double getUpperBound() const;
        double getLowerBound() const;
    };
}
#endif
