#include <assert.h>
#include "global.h"
#include "lagrbab.h"

#ifndef NDEBUG
static unsigned long long __TSPLagrNode_Counter = 0;
#endif

namespace TSPBaB {
    TSPLagrangianBaBTree::TSPLagrangianBaBTree(const Data& data, double upper) : BranchAndBound::Tree(upper), data(data) { }

    void TSPLagrangianBaBTree::solve() {
        BranchAndBound::Tree::solve(BranchAndBound::NodePtr(new TSPLagrNode(data)));
    }

    bool TSPLagrNode::solve(BranchAndBound::Tree* tree) {
        
        lagr.optimize(tree->getUpperBound(), 1, lagr.getData().getDimension()*10, lagr.getData().getDimension());
        std::vector< std::vector<int> > adj_list(lagr.getData().getDimension());
        auto s = lagr.extract_solution(adj_list);
        if (s.has_value()) { // feasible
            dbgprintf("TSPLagrNode #%llu (with %lu arcs) solved to feasibility with upperbound=%lf, against %lf of the B&B tree.\n",
                __TSPLagrNode_Counter++, lagr.getAmountOfDisabledArcs(), s.value().cost, tree->getUpperBound());
            assert(lagr.getSolutionLB() == s.value().cost);
            // if (s.value().cost <= tree->getUpperBound()) {
            //     tree->setBest(this, true);
            //     assert(s.value().cost == tree->getUpperBound());
            return true;
        } else {
            size_t argmax_v = 0;
            for (size_t i  = 0; i < adj_list.size(); i++) {
                if (adj_list[i].size() > adj_list[argmax_v].size())
                    argmax_v = i;
            }
            
            dbgprintf("TSPLagrNode #%llu (with %lu arcs) solved to infeasibility with lowerbound=%lf, against %lf of the B&B tree, spawning %lu nodes.\n",
                __TSPLagrNode_Counter++, lagr.getAmountOfDisabledArcs(), lagr.getSolutionLB(), tree->getUpperBound(), adj_list[argmax_v].size());
            for (const int j : adj_list[argmax_v]) {
                auto bp = new TSPLagrNode(lagr);
                bp->lagr.disable_edge(argmax_v, j);
                tree->branch(bp);
            }
        }
        return false;
    }
}