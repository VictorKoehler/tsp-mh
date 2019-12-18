#include <vector>
#include <cstdlib>
#include <limits>
#include <iostream>

using namespace std;

typedef vector<int>::iterator vecit;

class TSPSolution : public vector<int> {
   public:
    TSPSolution(uint d, double **m) : vector<int>(2), dimension(d), cost(0), matrizAdj(m)
    {
        this->at(0) = this->at(1) = 0;
    }

    uint dimension;
    double cost, **matrizAdj;

    double update_cost() {
        double ncost = 0;
        for (auto it = begin(); it != end() - 1; it++) {
            ncost += matrizAdj[*it][*(it+1)];
        }
        cost = ncost;
        return ncost;
    }

    /**
     * If this represents a valid solution.
     */
    inline bool completed() {
        return dimension + 1 == uint(size());
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
        return insertion_cost(n, *(p-1), *p);
    }

    inline double remotion_cost(const vecit& i) const {
        return matrizAdj[*(i-1)][*(i+1)] - matrizAdj[*(i-1)][*i] - matrizAdj[*i][*(i-1)];
    }

    inline double reinsertion_cost(const vecit& i, const vecit& p) const {
        return remotion_cost(i) + insertion_cost(*i, p);
    }

    void printSolution() {
        cout << "Solution:";
        for (auto e : *this) {
            cout << " " << e;
        }
        cout << endl;
    }
};

