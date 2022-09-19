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
#include "data.h"

namespace TSPMH {

    typedef std::vector<int>::iterator vecit;

    class TSPSolution : public std::vector<int> {
    public:
        static const uint route_start = 0;
        double cost;
        Data& data;

        TSPSolution(Data& data) : vector<int>(2), cost(0), data(data) {
            this->at(route_start) = this->at(route_start) = 0;
            this->reserve(size_t(data.getDimension()));
        }

        TSPSolution(const TSPSolution& obj) : vector<int>(obj), data(obj.data) {
            cost = obj.cost;
        }

        TSPSolution& operator=(const TSPSolution& t) {
            std::vector<int>::operator=(t);
            cost = t.cost;
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
            return data(i, n) + data(n, j) - data(i, j);
        }

        inline double insertion_cost(uint n, const vecit& p) const {
            return insertion_cost(n, *(p - 1), *p);
        }

        inline double remotion_cost(const vecit& i) const {
            return data(*(i - 1), *(i + 1)) - data(*(i - 1), *i) -
                data(*i, *(i - 1));
        }

        inline double reinsertion_cost(const vecit& i, const vecit& p) const {
            return remotion_cost(i) + insertion_cost(*i, p);
        }

        double update_cost() {
            double ncost = 0;
            for (auto it = begin(); it != end() - 1; it++) {
                assert(data(*it, *(it + 1)) == data(*(it + 1), *it));
                ncost += data(*it, *(it + 1));
            }
            cost = ncost;
            return ncost;
        }

        /**
         * If this represents a valid solution.
         */
        inline bool completed() { return data.getDimension() + 1 == int(size()); }

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

        StackedTSPSolution(Data& data) : TSPSolution(data) { }

        StackedTSPSolution& operator=(const StackedTSPSolution& t) {
            TSPSolution::operator=(t);
            reset_NeighborhoodMove();
            return *this;
        }

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