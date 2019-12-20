#ifndef __TSPSOLUTION_CLASS__
#define __TSPSOLUTION_CLASS__

#include <assert.h>
#include <cstdlib>
#include <iostream>
#include <limits>
#include <memory>
#include <stack>
#include <vector>

#include "tspneigh.h"
#include "mlpconcat.h"


namespace MLPMH {

    typedef std::vector<int>::iterator vecit;


    class MLPSolution : public std::vector<int> {
    public:
        static const uint route_start = 0;
        uint dimension;
        double cost, duration, **matrizAdj;
        std::stack<std::unique_ptr<NeighborhoodMove> > neighmoves;
        Concatenation **subseqConcatenation;


        MLPSolution(const MLPSolution& t) : vector<int>(t) {
            dimension = t.dimension;
            cost = t.cost;
            duration = t.duration;
            matrizAdj = t.matrizAdj;
            subseqConcatenation = t.subseqConcatenation;
        }

        MLPSolution(uint d, double** m) : vector<int>(2), dimension(d), cost(0), duration(0), matrizAdj(m), subseqConcatenation(NULL) {
            this->at(route_start) = this->at(route_start) = 0;
        }

        MLPSolution& operator=(const MLPSolution& t) {
            std::vector<int>::operator=(t);
            dimension = t.dimension;
            cost = t.cost;
            duration = t.duration;
            matrizAdj = t.matrizAdj;
            subseqConcatenation = t.subseqConcatenation;
            return *this;
        }



        inline double tsp_insertion_cost(uint n, uint i, uint j) const {
            return matrizAdj[i][n] + matrizAdj[n][j] - matrizAdj[i][j];
        }

        inline double tsp_insertion_cost(uint n, const vecit& p) const {
            return tsp_insertion_cost(n, *(p - 1), *p);
        }


        inline void update_cost() {
            cost = 0;
            duration = 0;
            for (auto it = begin(); it != end() - 1; it++) {
                assert(matrizAdj[*it][*(it + 1)] == matrizAdj[*(it + 1)][*it]);
                duration += matrizAdj[*it][*(it + 1)];
                cost += duration;
            }
        }

        inline bool update_cost_duration_same() {
            double c = cost, d = duration;
            update_cost();
            return c == cost && d == duration;
        }


        /**
         * If this represents a valid solution.
         */
        inline bool completed() { return dimension + 1 == uint(size()); }



        inline double push_NeighborhoodMove(std::unique_ptr<NeighborhoodMove> n) {
            n->apply(this);
            neighmoves.push(move(n));
            return cost;
        }

        inline double pop_NeighborhoodMove() {
            neighmoves.top()->undo(this);
            neighmoves.pop();
            return cost;
        }

        inline void popall_NeighborhoodMove() {
            while (!neighmoves.empty()) pop_NeighborhoodMove();
        }

        inline void reset_NeighborhoodMove() {
            while (!neighmoves.empty()) neighmoves.pop();
        }

        void printSolution() {
            static bool alert = true;
            if (alert) {
                std::cout << "ALERT: UPDATING SEQUENCE!" << std::endl;
                alert = false;
            }
            update_subseqConcatenation();

            
            double c = cost, d = duration;
            update_cost();
            std::cout << "Solution (" << c << "/" << cost << "; " << d << "/" << duration << "):";
            // cout << "Solution:";
            int p = -1;
            for (auto e : *this) {
                //if (p != -1) std::cout << " <" << matrizAdj[p][e] << ">";
                //p = e;
                std::cout << " " << e;
            }
            std::cout << std::endl;
        }


        void update_subseqConcatenation() {
            const auto subseq = subseqConcatenation;
            const auto mat = matrizAdj;
            const auto mat_at = this;

            uint sz = dimension + 1;
            for (uint len = 2; len <= sz; len++) { // Length varies between 1 and max=dimension
                // Index varies between 0 and the max it can reach before overflowing.
                for (uint i = 0; i <= sz - len; i++) {
                    uint j = i + len - 1; // Less one because it is inclusive.
                    subconc(i, j) = concat_fullb(i, j-1, j, j);
                    subconc(j, i) = concat_fullb(j, j, j-1, i);
                }
            }
        }

        void alloc_subseqConcatenation() {
            Concatenation* t = new Concatenation[(dimension+1)*(dimension+1)];
            subseqConcatenation = new Concatenation*[(dimension+1)];

            for (uint i = 0; i <= dimension; ++i)
                subseqConcatenation[i] = t + i * (dimension+1);
            
            const auto subseq = subseqConcatenation;
            subconc(0, 0) = {0, 0, 0};
            for (uint i = 1; i <= dimension; i++) {
                subconc(i, i) = {0, 0, 1};
            }
        }

        void dealloc_subseqConcatenation() {
            delete[] subseqConcatenation[0];
            delete[] subseqConcatenation;
            subseqConcatenation = NULL;
        }
    };

}

#endif