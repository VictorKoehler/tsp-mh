#include <fstream>
#include <iostream>
#include "data.h"
#include "tests.h"
#include "tspbab.h"
#include "lagrbab.h"
#include "TimePoint.hpp"

using namespace std;


template<bool guides>
void printData();
void realignData();

#define RSEED 1574733907

int main(int argc, char** argv) {
    srand(RSEED);

    TimePoint tp;
    int upper_bound = TSPBaB::INT_HIGH;
    if (argc == 3) {
        argc--;
        upper_bound = atoi(argv[2]);
    }

	// unique_ptr<Data> data(new Data(argc, argv[1]));
	Data data(argc, argv[1]);
	data.readData();

    // class DataPrimAdapter {
    //     public:
    //     const Data& d;

    //     public:
    //     class DataPrimAdapterView {
    //         const Data& d;
    //         const int i;
    //         public:
    //         DataPrimAdapterView(const Data& d, const int i) : d(d), i(i) {}
    //         inline double operator[](int j) const { return d.getDistance(i, j); }
    //     };

    //     DataPrimAdapter(const Data& d) : d(d) {}
    //     inline DataPrimAdapterView operator[](int i) const { return DataPrimAdapterView(d, i); }
    //     inline int size() const { return d.getDimension(); }
    // };
    // DataPrimAdapter dpa(data);
    // for (const auto [v1, v2] : TSPBaB::prims_algorithm(data)) {
    //     std::cout << v1 << "," << v2 << "  ";
    // }
    // std::cout << "\n";
    // exit(1);

    if (upper_bound == TSPBaB::INT_HIGH) {
        auto a = TSPMH::gils_rvnd(data, 1, 10);
        upper_bound = int(a.cost); // safety margin
        printf("Choosen upperbound by heurist: %d\n", upper_bound);
    }

    auto lag = TSPLagrangian::LagrangianTSP(data);
    lag.optimize(upper_bound, 1, 100, 5);
    auto lags = lag.extract_solution();
    std::cout << "LagrangianTSP finished. Total elapsed time: " << tp.diff_seconds() << "\n";
    if (lags.has_value()) {
        lags.value().printSolution();
        std::cout << "\n\n";
    } else {
        std::cout << "LAGRANGIAN DID NOT YIELD A VALID SOLUTION! lb=" << lag.getSolutionLB() << "\n";
    }

    TSPBaB::TSPLagrangianBaBTree lagrbab(data, upper_bound+1.1);
    lagrbab.solve();
    std::cout << "TSPLagrangianBaBTree finished. Total elapsed time: " << tp.diff_seconds() << "\n";
    if (lagrbab.getBest()) {
        std::cout << "LAGRANGIAN SOLUTION: lb=" << lagrbab.getLowerBound() << ", ub=" << lagrbab.getUpperBound() << "\n";
        dynamic_cast<TSPBaB::TSPLagrNode*>(lagrbab.getBest().get())->getSolution().extract_solution().value().printSolution();
    } else {
        std::cout << "LAGRANGIAN B&B DID NOT YIELD A VALID SOLUTION! lb=" << lag.getSolutionLB() << "\n";
    }

    // auto bab = TSPBaB::solveTSPBab(data.getDimension(), data.getMatrixCost(), upper_bound);
    // cout << "Prohibited arcs:";
    // for (auto a : bab->arcs) {
    //     cout << " " << a.first << "," << a.second;
    // }
    // cout << endl << "Tour:";
    // for (auto t : bab->subtour) {
    //     cout << " " << t;
    // }
    // cout << endl << "Cost: " << bab->lower_bound << endl;

    return 0;
}
