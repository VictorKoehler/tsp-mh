#include "tests.h"

using namespace std;
using namespace TSPMH;

double** matrizAdj;  // matriz de adjacencia
uint dimension;      // quantidade total de vertices

void simple_swap_test() {
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
}

void simple_best_swap_test() {
    auto a = solutionConstructor(dimension, matrizAdj);
    a.printSolution();
    SwapMove::swap_best(&a, true);
    a.printSolution();
    SwapMove::swap_best(&a, true);
    a.printSolution();
    a.pop_NeighborhoodMove();
    a.printSolution();
    a.pop_NeighborhoodMove();
    a.printSolution();
}

void simple_best_twoopt_test() {
    auto a = solutionConstructor(dimension, matrizAdj);
    a.printSolution();
    TwoOptMove::twoopt_best(&a, true);
    a.printSolution();
    TwoOptMove::twoopt_best(&a, true);
    a.printSolution();
    a.pop_NeighborhoodMove();
    a.printSolution();
    a.pop_NeighborhoodMove();
    a.printSolution();
}

void simple_best_reinserion_test(size_t len) {
    auto a = solutionConstructor(dimension, matrizAdj);
    a.printSolution();
    ReinsertionMove::reinsertion_best(&a, len, true);
    a.printSolution();
    ReinsertionMove::reinsertion_best(&a, len, true);
    a.printSolution();
    a.pop_NeighborhoodMove();
    a.printSolution();
    a.pop_NeighborhoodMove();
    a.printSolution();
}

void simple_copy_test() {
    auto a = solutionConstructor(dimension, matrizAdj);
    auto b = a;
    cout << "a "; a.printSolution();
    a.push_NeighborhoodMove(unique_ptr<NeighborhoodMove>(new SwapMove(1, 2)));
    cout << "a "; a.printSolution();
    cout << "b "; b.printSolution();
    a.pop_NeighborhoodMove();
    cout << "a "; a.printSolution();
    cout << "b "; b.printSolution();
}