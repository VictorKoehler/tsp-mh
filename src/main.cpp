#include <fstream>
#include <iostream>
#include "readData.h"
#include "tests.h"

using namespace std;

double** matrizAdj;  // matriz de adjacencia
uint dimension;      // quantidade total de vertices

template<bool guides>
void printData();
template <typename Unit>
void tsp();
template <typename Unit>
void realignData(Unit **dst);

#define RSEED 1574733907




int main(int argc, char** argv) {
    srand(RSEED);

    argc--;

    int dim;
    readData(argc, argv, &dim, &matrizAdj);
    dimension = uint(dim);

    if (argc[argv][0] == '0') {
        printf("intger\n");
        tsp<int>();
    }
    else {
        printf("double\n");
        tsp<double>();
    }

    return 0;
}

template <typename Unit>
void tsp() {
    Unit **dist = new Unit*[dimension];
    for (uint i = 0; i < dimension; i++ ) {
        dist[i] = new Unit[dimension];
    }

    realignData<Unit>(dist);

    auto a = TSPMH::gils_rvnd<Unit>(dimension, dist);
    cout << "COST: " << a.cost << "\nSEED: " << RSEED << "\nROUTE:";
    for (auto i : a) cout << " " << i;
    cout << endl;
}


template<bool guides>
void printData() {
    cout << "dimension: " << dimension << endl;
    if constexpr (guides) {
        printf("    ");
        for (uint j = 0; j < dimension; j++) {
            printf("%3u ", j);
        }
        printf("\n");
    }
    for (uint i = 0; i < dimension; i++) {
        if constexpr (guides) printf("%2u: ", i);
        for (uint j = 0; j < dimension; j++) {
            printf("%3.0lf ", matrizAdj[i][j]);
        }
        printf("\n");
    }
}

template <typename Unit>
void realignData(Unit **dst) {
    for (uint i = 0; i < dimension; i++) {
        for (uint j = 0; j < dimension; j++) {
            dst[i][j] = Unit(matrizAdj[i + 1][j + 1]);
        }
    }
}
