#include <fstream>
#include <iostream>
#include "readData.h"
#include "tests.h"

using namespace std;

double** matrizAdj;  // matriz de adjacencia
uint dimension;      // quantidade total de vertices

template<bool guides>
void printData();
void realignData();

int main(int argc, char** argv) {
    // std::vector<int> elems = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11};
    // // elems.insert(elems.end() - 2, elems.begin() + 1, elems.begin() + 4);
    // // elems.erase(elems.begin() + 1, elems.begin() + 4);
    // elems.insert(elems.begin() + 2, elems.end() - 5, elems.end() - 2);
    // elems.erase(elems.end() - 5, elems.end() - 2);
    // for (auto e : elems) std::cout << e << " ";
    // std::cout << std::endl;

    srand(time(NULL));

    int dim;
    readData(argc, argv, &dim, &matrizAdj);
    dimension = uint(dim);
    realignData();
    simple_best_swap_test();
    return 0;
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

void realignData() {
    for (uint i = 0; i < dimension; i++) {
        for (uint j = 0; j < dimension; j++) {
            matrizAdj[i][j] = matrizAdj[i + 1][j + 1];
        }
    }
}