#include "tests.h"

using namespace std;

extern double** matrizAdj;  // matriz de adjacencia
extern uint dimension;      // quantidade total de vertices

void simple_test() {
    auto a = solutionConstructor(dimension, matrizAdj);
    a.alloc_subseqConcatenation();
    a.update_subseqConcatenation();
    simple_best_swap_test(a);
}

void simple_swap_test(MLPSolution &a) {
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

void simple_best_swap_test(MLPSolution &a) {
    a.printSolution();
    SwapMove::swap_best(&a, true);
    a.printSolution();
    a.update_subseqConcatenation();
    SwapMove::swap_best(&a, true);
    a.printSolution();
    a.update_subseqConcatenation();
    a.pop_NeighborhoodMove();
    a.printSolution();
    a.update_subseqConcatenation();
    a.pop_NeighborhoodMove();
    a.printSolution();
}

void simple_best_twoopt_test(MLPSolution &a) {
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

void simple_best_reinserion_test(MLPSolution &a, size_t len) {
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

void simple_copy_test(MLPSolution &a) {
    auto b = a;
    cout << "a "; a.printSolution();
    a.push_NeighborhoodMove(unique_ptr<NeighborhoodMove>(new SwapMove(1, 2)));
    cout << "a "; a.printSolution();
    cout << "b "; b.printSolution();
    a.pop_NeighborhoodMove();
    cout << "a "; a.printSolution();
    cout << "b "; b.printSolution();
}