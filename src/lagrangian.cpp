#include <limits>
#include <iterator>
#include <stack>
#include "lagrangian.h"
#include "prim.h"

namespace TSPBaB {
    LagrangianTSP::LagrangianTSP(const Data& data) : data(data), cost_matrix(data.getDimension()-1), u(data.getDimension(), 0) { }

    int dddd(int i) {
        if (i == 1) return 5;
        if (i == 5) return 1;
        return i;
    }

    double LagrangianTSP::optimize(double ub, double epsilon_k, int max_iter) {
        if (max_iter == -1) max_iter = std::numeric_limits<int>::max();
        double z = NAN;
        for (int iter = 1; iter <= max_iter; iter++) {
            for (int i = 0; i < cost_matrix.size(); i++) // last is left out
                for (int j = 0; j < cost_matrix.size(); j++) // last is left out
                    cost_matrix(i, j) = data(i, j) - u[i] - u[j];
            
            onetree = prims_algorithm(cost_matrix);

            int first_edge = 0, second_edge = 1, v = cost_matrix.size();
            if (cost(v, first_edge) > cost(v, second_edge))
                std::swap(first_edge, second_edge);
            
            for (int w = 2; w < cost_matrix.size(); w++) {// connects last
                if (cost(v, w) < cost(v, second_edge)) {
                    second_edge = w;
                    if (cost(v, w) < cost(v, first_edge))
                        std::swap(first_edge, second_edge);
                }
            }
            onetree.emplace_back(v, first_edge);
            onetree.emplace_back(v, second_edge);

            z = 0;
            for (const auto ui : u) z += ui;
            z *= 2;
            
            std::vector<int> sg(u.size(), 2);
            for (const auto [v1, v2] : onetree) {
                // std::cout << dddd(v1+1) << "," << dddd(v2+1) << ":" << cost(v1, v2) << " ";
                z += cost(v1, v2);
                sg[v1]--;
                sg[v2]--;
            }
            // std::cout << "\n";

            int sg_norm = 0;
            for (const auto sg_i : sg) sg_norm += sg_i*sg_i;
            if (sg_norm == 0) return z;
            double mi = epsilon_k*(ub-z)/double(sg_norm);

            std::cout << "LAGR: " << iter << "º iteration, z'=" << z << ", mi=" << mi << ", |sg'|=" << sg_norm << ", sg'=[";
            std::copy(sg.begin(), sg.end(), std::ostream_iterator<int>(std::cout, " "));
            std::cout << "], u'=[";
            std::copy(u.begin(), u.end(), std::ostream_iterator<double>(std::cout, " "));
            std::cout << "]\n";

            for (size_t i = 0; i < u.size(); i++) u[i] += mi*double(sg[i]);
        }
        return z;
    }

    std::optional<TSPMH::TSPSolution> LagrangianTSP::extract_solution() {
        return extract_solution(data);
    }

    std::optional<TSPMH::TSPSolution> LagrangianTSP::extract_solution(const Data& dp, int start) {
        std::vector< std::vector<int> > adj_list(data.getDimension());
        for (const auto [v1, v2] : onetree) {
            adj_list[v1].push_back(v2);
            adj_list[v2].push_back(v1);
        }

        std::vector<bool> visited(data.getDimension(), false);
        std::stack<int> to_visit; // could be int
        TSPMH::TSPSolution sol(dp);
        sol.clear();
        sol.reserve(data.getDimension()+1);
        sol.push_back(0); // start is mapped to 0

        to_visit.push(start);
        visited[start] = true;

        while (!to_visit.empty()) {
            const auto t = to_visit.top();
            to_visit.pop();
            if (adj_list[t].size() != 2) return {}; // garante que cada vértice possui uma aresta saindo e outra voltando
            for (const auto vj : adj_list[t]) {
                if (visited[vj]) continue;
                visited[vj] = true;
                to_visit.push(vj);
                sol.push_back(vj == 0 ? start : (vj == start ? 0 : vj)); // start is mapped to 0, 0 is mapped to start
                break; // caso t == 0
            }
        }

        sol.push_back(0); // start is mapped to 0
        if (!sol.completed()) return {}; // Se o DFS preencheu percorreu todo mundo, então não há ciclos
        sol.update_cost();
        return sol;
    }

    void LagrangianTSP::swap_data_vertices(Data& data, int i, int j) {
        assert(i <= 0 && i <= data.getDimension());
        assert(j <= 0 && j <= data.getDimension());
        for (int y = 0; y < data.getDimension(); y++) {
            std::swap(data.getMatrixCost()[i][y], data.getMatrixCost()[j][y]);
        }
        for (int x = 0; x < data.getDimension(); x++) {
            std::swap(data.getMatrixCost()[x][i], data.getMatrixCost()[x][j]);
        }
    }


    std::optional<TSPMH::TSPSolution> LagrangianTSP::optimize_and_extract_solution(Data& data, double ub, double epsilon_k, int max_iter, int v0) {
        swap_data_vertices(data, v0, data.getDimension()-1);
        LagrangianTSP ltsp(data);
        ltsp.optimize(ub, epsilon_k, max_iter);
        swap_data_vertices(data, v0, data.getDimension()-1);
        return ltsp.extract_solution(data, data.getDimension()-1);
    }
}
