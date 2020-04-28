#include "gianttour.h"
#include "../global.h"

using namespace std;
using namespace HGSADC;

GiantTour::GiantTour(CVRPMH::CVRPSolution& source) {
    data = source.data;
    tour.push_back(CVRPMH::CVRPSolution::route_start);
    for (auto i : source) {
        if (i != CVRPMH::CVRPSolution::route_start)
            tour.push_back(i);
    }
}

CVRPMH::CVRPSolution GiantTour::split_back() {
    return split_back_bellman_restricted_viability();
}

CVRPMH::CVRPSolution GiantTour::split_back_bellman_restricted_viability() {
    return split_back_bellman_penalized_viability(0, 1);
}

CVRPMH::CVRPSolution GiantTour::split_back_bellman_penalized_viability(double wpen, int qmult) {
    CVRPMH::CVRPSolution ret(data);
    ret.subroutes.resize(ret.data->vehicles);
    ret.subcapacity.resize(ret.data->vehicles);
    ret.resize(ret.data->vehicles + ret.data->dimension);

    // https://w1.cirrelt.ca/~vidalt/presentations/Vidal-Linear-Split.pdf :: 8
    assert(size_t(ret.data->dimension) == tour.size());
    assert(tour[0] == CVRPMH::CVRPSolution::route_start);

    const auto n = ret.data->dimension-1;
    const auto q = ret.data->maxcapacity;
    uint pred[ret.data->dimension] = {0};
    vector<double> p(tour.size(), INFINITYLF); // 3
    p[0] = 0; // 1

    for (uint t = 0; t < n; t++) { // 4
        uint j = t+1; // 6
        int load = 0; // 5
        double cost = 0;

        while (j <= n && load + ret.data->demand[tour[j]] <= qmult*q) { // 7
            load += ret.data->demand[tour[j]]; // 8
            if (j == t+1) // 9
                cost = ret.data->matrizAdj[0][tour[j]]; // 10
            else // 11
                cost += ret.data->matrizAdj[tour[j-1]][tour[j]]; // 12

            auto minor = p[t] + cost + ret.data->matrizAdj[tour[j]][0] + wpen*max(0, load-q);
            if (minor < p[j]) { // 13
                p[j] = minor; // 14
                pred[j] = t; // 15
            }
            j++; // 16
        }
    }

    // for (uint t = 0; t < ret.dimension; t++) {
    //     auto a = pred[t];
    //     cout << "{" << tour[t] << "[" << t << "], p=" << tour[a] << "[" << a << "]}  ";
    // }
    // cout << endl;
    
    size_t inspos = ret.size()-1, predcur = ret.data->dimension-1, insertzero = predcur;
    for (; inspos > 0; inspos--) {
        if (predcur == insertzero) {
            ret[inspos] = 0;
            insertzero = pred[predcur];
        } else {
            ret[inspos] = tour[predcur--];
        }
    }
    ret[0] = 0;
    ret.updateRoutes(); // TODO: optimize
    ret.cost = p[p.size()-1];

    return ret;
}