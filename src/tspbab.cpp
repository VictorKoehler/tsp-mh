#include "tspbab.h"
#include <assert.h>

using namespace BranchAndBound;
using namespace std;


#define it(i) begin() + (i)

#ifndef NDEBUG
#define dprintf(...) printf(__VA_ARGS__)
#define DEBUGSTATMENT(a) a
#else
#define dprintf(...)
#define DEBUGSTATMENT(a)
#endif


namespace TSPBaB {

    inline TSPNode* hung_subt_dec(BranchAndBound::Tree* tree) {
        // Then we solve the Hungarian Problem.
        //hungarian_print_costmatrix(&tree->hp);
        double lb = tree->solve_hungarian_problem();

        // Throw away if this solution has a bad lower_bound.
        if (lb >= tree->upper_bound) return NULL;

        // We swap tmp0 with tmp1, so we don't need to copy every time we find a good subtour.
        vector<int> tmp0(tree->dimension + 1), tmp1(tree->dimension + 1), *choosen = &tmp0, *bkp = &tmp1;
        uint choosensz = INT_HIGH, visitedcount = 0;
        bool visited[tree->dimension] = { false };

        // We look for every subtour
        while (visitedcount != tree->dimension) {
            uint s, i, ciclecount = 0; // s is the subtour start, i is the i-th position.
            for (s = 0; visited[s]; s++) {
                assert(s != tree->dimension); // Should never happen, because of while statement.
            }
            i = s;
            (*bkp)[ciclecount++] = i; // We start from the start.
            // ~visitedcount++;~ the above do-while corrects it

            do {
                uint c;
                for (c = 0; tree->assignmentMatrix[i][c] == 0; c++) {
                    assert(c != tree->dimension); // Should never happen, because of the correctness of the algorithm
                }
                (*bkp)[ciclecount++] = c;
                visitedcount++;
                visited[c] = true;
                i = c;
            } while (i != s); // If we returned to start, then it is the end.

            if (ciclecount < choosensz) {
                // If this is a good solution, we say that choosen is this.
                // We don't need to copy the values, bkp and choosen will swap.
                std::vector<int> *t = choosen;
                choosen = bkp;
                bkp = t;
                choosensz = ciclecount;
            }
        }

        assert(choosensz <= tree->dimension + 1);
        choosen->resize(choosensz); // Truncate the vector
        TSPNode* child = new TSPNode(lb);
        child->subtour = *choosen; // Now we copy
        return child;
    }

    void TSPNode::solve(Tree* tree) {
        #ifndef NDEBUG
        printf("TSPNode::solve - Lower_bound=%d;  Height=%lu;  Subtour Arcs=%lu;", this->lower_bound, arcs.size(), subtour.size());
        int cc_branches = 0, cc_leafs = 0;
        #endif

        // Block arcs
        vector<int> deleted_arcs(arcs.size());
        for (size_t i = 0; i < arcs.size(); i++) {
            const auto a = arcs[i];
            deleted_arcs[i] = tree->costMatrix[a.first][a.second];
            tree->costMatrix[a.first][a.second] = INT_HIGH;
        }

        // For every arc of the subtour associated with this node, branch OR prune.
        for (auto k = subtour.it(1); k != subtour.end(); k++) {
            // Along with the others blocked/penalized arcs, add the arc k temporarily.
            const auto a = make_pair(*(k-1), *k);
            const int deleted_arc = tree->costMatrix[a.first][a.second];
            tree->costMatrix[a.first][a.second] = INT_HIGH;

            // cout << "\nProhibited arcs: ";
            // for (auto as : arcs) {
            //     cout << as.first << "," << as.second << " ";
            // }
            // cout << a.first << "," << a.second << "\n";

            TSPNode* child = hung_subt_dec(tree);
            if (child != NULL) {
                child->arcs = arcs;
                child->arcs.push_back(a);
                // The subtour vector has size S, but the first and last elements are the start.
                // If choosensz == tree->dimension + 1, then exists only one subtour.
                if (uint(child->subtour.size()) == tree->dimension + 1) {
                    // We visited all vertex. Since this-node-lb < tree->upper_bound,
                    // we say that this is the best solution so far, and set tree->upper_bound accordingly.
                    tree->setBest(child, true);
                    DEBUGSTATMENT(cc_leafs++);
                } else {
                    // No subtour => Branch
                    tree->branch(child);
                    DEBUGSTATMENT(cc_branches++);
                }
            }

            // Restore the k arc
            tree->costMatrix[a.first][a.second] = deleted_arc;
        }
        dprintf("  Branches=%d;  Leafs=%d\n", cc_branches, cc_leafs);

        // Unblock arcs
        for (size_t i = 0; i < arcs.size(); i++) {
            const auto a = arcs[i];
            tree->costMatrix[a.first][a.second] = deleted_arcs[i];
        }
    }


    shared_ptr<TSPNode> solveTSPBab(int dimension, double **matrix, int upper_bound) {
        Tree tree(dimension, matrix, upper_bound);
        auto root = hung_subt_dec(&tree);
        if (uint(root->subtour.size()) == dimension + 1u) {
            return shared_ptr<TSPNode>(root);
        }
        else tree.solve(NodePtr(root));
        return dynamic_pointer_cast<TSPNode>(tree.best_node);
    }
}