#define CVRPPOOL_ENABLED

#include <fstream>
#include <iostream>
#include "tests.h"
#include "tsp/readData.h"
#include "tsp/neighmoves.h"
#include "cvrp/heur.h"
#include "cvrp/neighmoves.h"

using namespace std;

double** matrizAdj;  // matriz de adjacencia
uint dimension;      // quantidade total de vertices

template<bool guides>
void printData();
void realignData();

#define RSEED 1574733907



int tsp(int argc, char** argv) {
    int dim;
    readData(argc, argv, &dim, &matrizAdj);
    dimension = uint(dim);
    realignData();
    auto a = TSPMH::gils_rvnd(dimension, matrizAdj);
    cout << " COST: " << a.cost << "\nSEED: " << RSEED << "\nROUTE:";
    for (auto i : a) cout << " " << i;
    cout << endl;
    return 0;
}



int cvrp(int argc, char** argv) {
    char nome[100];
    auto a = CVRPMH::gils_rvnd(std::unique_ptr<LegacyCVRP::Instancia>(LegacyCVRP::lerInstancia(fopen(argv[1], "r"), nome)));
    cout << CVRPMH::cvrp_nmcc_swap << " " << CVRPMH::cvrp_nmcc_reinsertion << " " <<
        TSPMH::tsp_nmcc_swap << " " << TSPMH::tsp_nmcc_reinsertion << " " << TSPMH::tsp_nmcc_twoopt << "\n";
    if (!a.checkSolution()) cout << "Invalid solution\n";
    cout << "COST: " << a.cost << "\nSEED: " << RSEED << "\nROUTE:";
    for (auto i : a) cout << " " << i << "(" << a.demand[i] << ")";
    cout << endl;
    return 0;
}



int main(int argc, char** argv) {
    TSPMH::_random_seed(RSEED);

    ifdebug(printf("WARNING: DEBUG ENABLED!\n"));
    assert(printf("WARNING: ASSERTIVES ENABLED!\n") >= 0);
    dprintf("WARNING: VERBOSITY ENABLED!\n");
    cvrp(argc, argv);
}

// #include <iostream>     // std::cout
// #include <algorithm>    // std::lower_bound, std::upper_bound, std::sort
// #include <vector>       // std::vector

// int main () {
//     std::vector<int> v = {0, 9, 10}, k = {0, 1, 8, 9, 10, 11};

//     for (auto i : k) {
//         std::vector<int>::iterator low,up;
//         low=std::lower_bound (v.begin(), v.end(), i); //          ^
//         up= std::upper_bound (v.begin(), v.end(), i); //                   ^

//         std::cout << i << ": lower_bound at " << (low- v.begin()) << "; upper_bound at " << (up - v.begin()) << '\n';
//     }

//     return 0;
// }

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
