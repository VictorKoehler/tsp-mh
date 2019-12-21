#include <fstream>
#include <iostream>
#include "data.h"
#include "tests.h"
#include "hungarian.h"
#include "bab.h"

using namespace std;


template<bool guides>
void printData();
void realignData();

#define RSEED 1574733907

int main(int argc, char** argv) {
    srand(RSEED);

	hungarian_problem_t p;
	int mode = HUNGARIAN_MODE_MINIMIZE_COST;
	hungarian_init(&p, data->getMatrixCost(), data->getDimension(), data->getDimension(), mode); // Carregando o problema

	double obj_value = hungarian_solve(&p);
	cout << "Obj. value: " << obj_value << endl;

	cout << "Assignment" << endl;
	hungarian_print_assignment(&p);

	hungarian_free(&p);

    auto a = TSPMH::gils_rvnd(dimension, matrizAdj);
    cout << "COST: " << a.cost << "\nSEED: " << RSEED << "\nROUTE:";
    for (auto i : a) cout << " " << i;
    cout << endl;
    return 0;
}
