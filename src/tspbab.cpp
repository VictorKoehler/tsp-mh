#include "tspbab.h"
#include <assert.h>
#include "global.h"

using namespace BranchAndBound;
using namespace std;


#define it(i) begin() + (i)

namespace TSPBaB {

    #define __copymatrix(src, dst, dimx, dimy) \
    for (uint i = 0; i < dimx; i++) { \
        for (uint j = 0; j < dimy; j++) { \
            dst[i][j] = src[i][j]; \
        } \
    }

    HungarianBaBTree::HungarianBaBTree(int d, double **m, int upper) : BranchAndBound::Tree(upper), dimension(d) {
        hungarian_init(&hp, m, d, d, HUNGARIAN_MODE_MINIMIZE_COST);
        costMatrix = hp.cost;
        assignmentMatrix = hp.assignment;
    }

    HungarianBaBTree::~HungarianBaBTree() {
        costMatrix = NULL;
        assignmentMatrix = NULL;
        hungarian_free(&hp);
    }

    TSPNode* HungarianBaBTree::hung_subt_dec() {
        // Then we solve the Hungarian Problem.
        //hungarian_print_costmatrix(&tree->hp);
        int bkpMatrix[dimension][dimension];
        __copymatrix(hp.cost, bkpMatrix, dimension, dimension);
        double lb = hungarian_solve(&hp);
        __copymatrix(bkpMatrix, hp.cost, dimension, dimension);

        // Throw away if this solution has a bad lower_bound.
        if (lb >= this->upper_bound) return NULL;

        // We swap tmp0 with tmp1, so we don't need to copy every time we find a good subtour.
        vector<int> tmp0(this->dimension + 1), tmp1(this->dimension + 1), *choosen = &tmp0, *bkp = &tmp1;
        uint choosensz = INT_HIGH, visitedcount = 0;
        bool visited[this->dimension] = { false };

        // We look for every subtour
        while (visitedcount != this->dimension) {
            uint s, i, ciclecount = 0; // s is the subtour start, i is the i-th position.
            for (s = 0; visited[s]; s++) {
                assert(s != this->dimension); // Should never happen, because of while statement.
            }
            i = s;
            (*bkp)[ciclecount++] = i; // We start from the start.
            // ~visitedcount++;~ the above do-while corrects it

            do {
                uint c;
                for (c = 0; this->assignmentMatrix[i][c] == 0; c++) {
                    assert(c != this->dimension); // Should never happen, because of the correctness of the algorithm
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

        assert(choosensz <= this->dimension + 1);
        choosen->resize(choosensz); // Truncate the vector
        TSPNode* child = new TSPNode(lb);
        child->subtour = *choosen; // Now we copy
        return child;
    }

    void TSPNode::solve(HungarianBaBTree* tree) {
        #ifndef NDEBUG
        printf("TSPNode::solve - Lower_bound=%lf;  Height=%lu;  Subtour Arcs=%lu;", this->lower_bound, arcs.size(), subtour.size());
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

            TSPNode* child = tree->hung_subt_dec();
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
        dbgprintf("  Branches=%d;  Leafs=%d\n", cc_branches, cc_leafs);

        // Unblock arcs
        for (size_t i = 0; i < arcs.size(); i++) {
            const auto a = arcs[i];
            tree->costMatrix[a.first][a.second] = deleted_arcs[i];
        }
    }

    shared_ptr<TSPNode> TSPNode::solveTSPBab(int dimension, double **matrix, int upper_bound) {
        HungarianBaBTree tree(dimension, matrix, upper_bound);
        auto root = tree.hung_subt_dec();
        if (uint(root->subtour.size()) == dimension + 1u) {
            return shared_ptr<TSPNode>(root);
        }
        else tree.solve(NodePtr(root));
        return dynamic_pointer_cast<TSPNode>(tree.best_node);
    }

    shared_ptr<TSPNode> solveTSPBab(int dimension, double **matrix, int upper_bound) {
        return TSPNode::solveTSPBab(dimension, matrix, upper_bound);
    }
}