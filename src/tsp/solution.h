#ifndef __TSPSOLUTION_CLASS__
#define __TSPSOLUTION_CLASS__

#include <cstdlib>
#include <iostream>
#include <limits>
#include <memory>
#include <stack>
#include <vector>

#include "neigh.h"
#include "contextdata.h"

namespace TSPMH {

    typedef std::vector<int>::iterator vecit;

    class TSPSolution : public std::vector<int> {
    public:
        static const uint route_start = TSPContextProblemData::route_start;
        double cost;
        TSPContextProblemData *data;
        bool destroydata;

        TSPSolution() : destroydata(false) {}

        TSPSolution(TSPContextProblemData* data) : vector<int>(2), cost(0), data(data), destroydata(false) {
            this->at(route_start) = this->at(route_start) = 0;
            this->reserve(size_t(data->dimension));
        }

        TSPSolution(const TSPSolution& obj) : vector<int>(obj), destroydata(false) {
            data = obj.data;
            cost = obj.cost;
        }

        TSPSolution& operator=(const TSPSolution& t) {
            std::vector<int>::operator=(t);
            cost = t.cost;
            data = t.data;
            return *this;
        }

        ~TSPSolution() {
            if (destroydata) delete data;
        }


        virtual int &at(std::size_t __n) { return std::vector<int>::at(__n); }
        virtual const int operator[](std::size_t index) const noexcept { return std::vector<int>::operator[](index); }
        virtual int& operator[](std::size_t index) noexcept { return std::vector<int>::operator[](index); }
        virtual vecit begin() noexcept { return std::vector<int>::begin(); }
        virtual vecit end() noexcept { return std::vector<int>::end(); }
        virtual void push_back(int x) { std::vector<int>::push_back(x); }
        virtual vecit insert(const vecit p, int i)  { return std::vector<int>::insert(p, i); }
        virtual vecit insert(const vecit p, const vecit b, const vecit e)  { return std::vector<int>::insert(p, b, e); }
        virtual vecit erase(const vecit b, const vecit e)  { return std::vector<int>::erase(b, e); }
        virtual std::size_t size() const noexcept { return std::vector<int>::size(); }
        virtual vecit it(std::size_t ind) { return begin() + ind; }
        virtual vecit it(int ind) { return begin() + ind; }



        inline void insert_candidate(int c, const vecit& pos) {
            cost += insertion_cost(c, pos);
            insert(pos, c);
        }

        inline void insert_candidate(int c, int pos) {
            insert_candidate(c, begin() + pos);
        }

        inline double insertion_cost(uint n, uint i, uint j) const {
            return data->matrizAdj[i][n] + data->matrizAdj[n][j] - data->matrizAdj[i][j];
        }

        inline double insertion_cost(uint n, const vecit& p) const {
            return insertion_cost(n, *(p - 1), *p);
        }

        inline double remotion_cost(const vecit& i) const {
            return data->matrizAdj[*(i - 1)][*(i + 1)] - data->matrizAdj[*(i - 1)][*i] -
                data->matrizAdj[*i][*(i - 1)];
        }

        inline double reinsertion_cost(const vecit& i, const vecit& p) const {
            return remotion_cost(i) + insertion_cost(*i, p);
        }

        double update_cost() {
            double ncost = 0;
            for (auto it = begin(); it != end() - 1; it++) {
                assert(data->matrizAdj[*it][*(it + 1)] == data->matrizAdj[*(it + 1)][*it]);
                ncost += data->matrizAdj[*it][*(it + 1)];
            }
            cost = ncost;
            return ncost;
        }

        /**
         * If this represents a valid solution.
         */
        inline bool completed() { return data->dimension + 1 == uint(size()); }


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

        StackedTSPSolution(TSPContextProblemData* data) : TSPSolution(data) { }

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