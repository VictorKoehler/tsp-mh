#include <fstream>
#include <iostream>
#include "data.h"
#include "tests.h"
#include "tspbab.h"

using namespace std;


template<bool guides>
void printData();
void realignData();

#define RSEED 1574733907

int main(int argc, char** argv) {
    srand(RSEED);

    int upper_bound = BranchAndBound::INT_HIGH;
    if (argc == 3) {
        argc--;
        upper_bound = atoi(argv[2]);
    }

	// unique_ptr<Data> data(new Data(argc, argv[1]));
	Data data(argc, argv[1]);
	data.readData();

    if (upper_bound == BranchAndBound::INT_HIGH) {
        auto a = TSPMH::gils_rvnd(data, 1, 10);
        upper_bound = int(a.cost) + 5; // safety margin
        printf("Choosen upperbound by heurist: %d\n", upper_bound);
    }

    auto bab = TSPBaB::solveTSPBab(data.getDimension(), data.getMatrixCost(), upper_bound);
    cout << "Prohibited arcs:";
    for (auto a : bab->arcs) {
        cout << " " << a.first << "," << a.second;
    }
    cout << endl << "Tour:";
    for (auto t : bab->subtour) {
        cout << " " << t;
    }
    cout << endl << "Cost: " << bab->lower_bound << endl;

    return 0;
}
