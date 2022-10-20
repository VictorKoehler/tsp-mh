#include "bab.h"

namespace BranchAndBound {
    std::string timeStamp() {
        std::ostringstream strStream;
        std::time_t t = std::time(nullptr);
        strStream<< std::put_time(std::localtime(&t), "%F %T %Z");
        return strStream.str();
    }

    void Tree::solve(NodePtr root) {
        int iter_cc = 0, solutions = 0;
        setBest(root, false);
        nodes.push(std::move(root));

        auto log = [&](const NodePtr& n) {
            using std::setw;
            const auto dp{std::cout.precision()};
            const auto gap = (upper_bound - n->lb()) / upper_bound * 100.0;
            std::cout << setw(6) << iter_cc << setw(13) << nodes.size() << setw(11) << n->lb() << setw(11) << upper_bound << setw(9)
                      << std::setprecision(4) << gap << "% " << setw(5) << solutions << "   " << timeStamp() << "\n" << std::setprecision(dp);
        };

        std::cout << "B&B Tree:\n  iter | Nodes Left |       lb |       ub |     gap | sol | time\n";
        while (!nodes.empty()) {
            NodePtr n = std::move(nodes.top());
            nodes.pop();
            if (iter_cc % 250 == 0) log(n);
            if (n->lb() > upper_bound) {
                nodes = pqueue();
                break;
            }
            if (n->solve(this)) {
                if (n->lb() < upper_bound) setBest(n, true);
                solutions++;
                log(n);
            }
            iter_cc++;
        }
    }

    void Tree::branch(NodePtr node) {
        nodes.push(node);
    }

    void Tree::branch(Node* node) {
        branch(NodePtr(node));
    }

    void Tree::setBest(NodePtr node, bool setUB) {
        best_node = node;
        if (setUB) {
            upper_bound = best_node->lb();
        }
    }

    void Tree::setBest(Node* node, bool setUB) {
        setBest(NodePtr(node), setUB);
    }

    const NodePtr& Tree::getBest() const {
        return best_node;
    }

    double Tree::getUpperBound() const {
        return upper_bound;
    }

    double Tree::getLowerBound() const {
        return getBest() ? getBest()->lb() : -std::numeric_limits<double>::infinity();
    }
}
