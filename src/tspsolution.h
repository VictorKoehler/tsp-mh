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


namespace TSPMH {

    typedef std::vector<int>::iterator vecit;

    class TSPSolution : public std::vector<int> {
    public:
        static const uint route_start = 0;
        uint dimension;
        double cost, **matrizAdj;

        TSPSolution(const TSPSolution& obj) : vector<int>(obj) {
            dimension = obj.dimension;
            cost = obj.cost;
            matrizAdj = obj.matrizAdj;
        }

        TSPSolution(uint d, double** m) : vector<int>(2), dimension(d), cost(0), matrizAdj(m) {
            this->at(route_start) = this->at(route_start) = 0;
        }

        TSPSolution& operator=(const TSPSolution& t) {
            std::vector<int>::operator=(t);
            dimension = t.dimension;
            cost = t.cost;
            matrizAdj = t.matrizAdj;
            return *this;
        }



        inline void insert_candidate(int c, const vecit& pos) {
            cost += insertion_cost(c, pos);
            insert(pos, c);
        }

        inline void insert_candidate(int c, int pos) {
            insert_candidate(c, begin() + pos);
        }

        inline double insertion_cost(uint n, uint i, uint j) const {
            return matrizAdj[i][n] + matrizAdj[n][j] - matrizAdj[i][j];
        }

        inline double insertion_cost(uint n, const vecit& p) const {
            return insertion_cost(n, *(p - 1), *p);
        }

        inline double remotion_cost(const vecit& i) const {
            return matrizAdj[*(i - 1)][*(i + 1)] - matrizAdj[*(i - 1)][*i] -
                matrizAdj[*i][*(i - 1)];
        }

        inline double reinsertion_cost(const vecit& i, const vecit& p) const {
            return remotion_cost(i) + insertion_cost(*i, p);
        }

        double update_cost() {
            double ncost = 0;
            for (auto it = begin(); it != end() - 1; it++) {
                assert(matrizAdj[*it][*(it + 1)] == matrizAdj[*(it + 1)][*it]);
                ncost += matrizAdj[*it][*(it + 1)];
            }
            cost = ncost;
            return ncost;
        }

        /**
         * If this represents a valid solution.
         */
        inline bool completed() { return dimension + 1 == uint(size()); }


        void printSolution() {
            double d = cost;
            std::cout << "Solution (" << d << "/" << update_cost() << "):";
            // cout << "Solution:";
            for (auto e : *this) {
                std::cout << " " << e;
            }
            std::cout << std::endl;
        }
    };

    class StackedTSPSolution : public TSPSolution {
        public:
        std::stack<std::unique_ptr<NeighborhoodMove> > neighmoves;

        StackedTSPSolution(const StackedTSPSolution& obj) : TSPSolution(obj) { }

        StackedTSPSolution(uint d, double** m) : TSPSolution(d, m) { }

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
    };

}
#endif