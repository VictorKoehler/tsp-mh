#include <fstream>
#include <iostream>
#include "readData.h"
#include "tspheur.h"

using namespace std;

double** matrizAdj;  // matriz de adjacencia
uint dimension;      // quantidade total de vertices

template<bool guides>
void printData();
void realignData();

int main(int argc, char** argv) {
    srand(time(NULL));

    int dim;
    readData(argc, argv, &dim, &matrizAdj);
    dimension = uint(dim);
    realignData();
    auto a = solutionConstructor(dimension, matrizAdj);
    a.printSolution();
    a.push_NeighborhoodMove(unique_ptr<NeighborhoodMove>(new SwapMove(1, 2)));
    a.printSolution();
    a.push_NeighborhoodMove(unique_ptr<NeighborhoodMove>(new SwapMove(4, 5)));
    a.printSolution();
    a.pop_NeighborhoodMove();
    a.printSolution();
    a.pop_NeighborhoodMove();
    a.printSolution();
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