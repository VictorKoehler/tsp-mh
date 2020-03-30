#include <iostream>
#include "pertubation_metric.h"
#include "global.h"

double CommonEdgeDifferenceMetric::stackSolution(std::vector<int>& flattenRoutesSolution) {
    // Se não há solução anterior: esta será a solução anterior.
    if (previousSolution.size() == 0) {
        previousSolution = flattenRoutesSolution;
        return 0;
    }

    assert(previousSolution.size() == flattenRoutesSolution.size());

    // Parâmetro especial, se o número de clientes não foi fornecido ao construtor,
    // então ele será obtido uma única vez aqui:
    if (numClients == 0) {
        // Obtém o cliente que tem maior índice, esse é o número de clientes + 1.
        for (auto i = previousSolution.begin()+1; i != previousSolution.end(); i++) {
            if (unsigned(*i) > numClients) {
                numClients = unsigned(*i);
            }
        }
        numClients++;
    }

    // Mapeamos a "solução anterior" para um array, marcando as arestas visitadas.
    uint8_t previousMap[numClients][numClients] = {0};
    int previousClient = 0;
    for (auto i = previousSolution.begin()+1; i != previousSolution.end(); i++) {
        previousMap[previousClient][*i] = 1;

        // Assume-se que o problema é simétrico (Two-Opt é o mais atingido)
        previousMap[*i][previousClient] = 1;
        previousClient = *i;
    }

    // Com a solução anterior mapeada, varremos a solução atual e verificamos se ela possui
    // as mesmas arestas.
    unsigned int commonEdges = 0, totalEdges = 0;
    for (auto i = flattenRoutesSolution.begin()+1; i != flattenRoutesSolution.end(); i++) {
        if (previousMap[previousClient][*i] == 1) {
            commonEdges++; // Se possui a mesma aresta, incrementa.
        }
        totalEdges++;
        previousClient = *i;
    }
    
    // Razão de repetição pelo total de arestas.
    double ratio = double(commonEdges)/double(totalEdges);
    sum += ratio;
    numInterations++;

    if (ratio == 1.0) onesSum++; // Toda vez que encontramos uma solução idêntica, incrementamos.
    if (keepHistory) history.push_back(ratio);
    previousSolution = flattenRoutesSolution; // Solução anterior é atualizada
    return ratio;
}

void CommonEdgeDifferenceMetric::unstack() {
    previousSolution.clear();
}

double CommonEdgeDifferenceMetric::average() {
    return sum/double(numInterations);
}

double CommonEdgeDifferenceMetric::ones_ratio() {
    return double(onesSum)/double(numInterations);
}

void CommonEdgeDifferenceMetric::printResults() {
    std::cout << "Pertubation Effectiveness (Common Edges Ratio):\n";

    if (keepHistory) {
        std::cout << "History:";
        for (auto i : history) {
            std::cout << " " << i;
        }
        std::cout << "\n";
    }

    std::cout << "Average: " << average() << "\n";
    std::cout << "Ones ratio: " << ones_ratio() << " (" << onesSum << "/" << numInterations << ")\n";
}
