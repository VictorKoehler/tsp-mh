#include <fstream>
#include <iostream>
#include "readData.h"

using namespace std;

double** matrizAdj;  // matriz de adjacencia
uint dimension;      // quantidade total de vertices

void printData();
void realignData();

int main(int argc, char** argv) {
    int dim;
    readData(argc, argv, &dim, &matrizAdj);
    dimension = uint(dim);
    realignData();
    return 0;
}

void printData() {
    cout << "dimension: " << dimension << endl;
    for (uint i = 1; i <= dimension; i++) {
        for (uint j = 1; j <= dimension; j++) {
            cout << matrizAdj[i][j] << " ";
        }
        cout << endl;
    }
}

void realignData() {
    for (uint i = 0; i < dimension; i++) {
        for (uint j = 0; j < dimension; j++) {
            matrizAdj[i][j] = matrizAdj[i + 1][j + 1];
        }
    }
}