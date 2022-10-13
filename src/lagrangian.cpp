#include <limits>
#include <iterator>
#include <stack>
#include "global.h"
#include "lagrangian.h"
#include "prim.h"
#include "matrix.h"


#define cost(i, j) (i == cost_matrix.size() ? data(i, j) - u[i] - u[j] : cost_matrix(i, j))


namespace TSPBaB {
    LagrangianTSP::LagrangianTSP(const Data& data) : data(data), best_u(data.getDimension(), 0) { }

    LagrangianTSP::LagrangianTSP(const LagrangianTSP& parent) : data(parent.data), best_u(parent.best_u) {
    }

    double LagrangianTSP::optimize(double ub, double epsilon_k, int max_iter, int max_iter_without_improv) {
        Simple2DMatrix<double> cost_matrix(data.getDimension()-1);
        std::vector<double> u;
        GraphEdges onetree;

        best_lb = -std::numeric_limits<double>::infinity();
        u.swap(best_u);

        if (max_iter == -1) max_iter = std::numeric_limits<int>::max();
        if (max_iter_without_improv == -1) max_iter_without_improv = std::numeric_limits<int>::max();

        for (int iter = 1, iter_wo_improv = 0; iter <= max_iter && iter_wo_improv <= max_iter_without_improv; iter++) {
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

            double nz = 0;
            for (const auto ui : u) nz += ui;
            nz *= 2;
            
            std::vector<int> sg(u.size(), 2);
            for (const auto [v1, v2] : onetree) {
                // std::cout << dddd(v1+1) << "," << dddd(v2+1) << ":" << cost(v1, v2) << " ";
                nz += cost(v1, v2);
                sg[v1]--;
                sg[v2]--;
            }
            // std::cout << "\n";
            if (nz > best_lb) {
                best_lb = nz;
                best_u = u;
                best_onetree.swap(onetree);
                iter_wo_improv = 0;
            } else {
                iter_wo_improv++;
            }


            int sg_norm = 0;
            for (const auto sg_i : sg) sg_norm += sg_i*sg_i;
            if (sg_norm == 0) return nz;
            double mi = epsilon_k*(ub-nz)/double(sg_norm);

            if (Verbosity >= 0) {
                std::cout << "LAGR: " << iter << "º iteration, z'=" << nz << ", mi=" << mi
                          << ", e_k=" << epsilon_k << ", |sg'|=" << sg_norm;
                if (Verbosity >= 3) {
                    std::cout << ", sg'=[";
                    std::copy(sg.begin(), sg.end(), std::ostream_iterator<int>(std::cout, " "));
                    std::cout << "], u'=[";
                    std::copy(u.begin(), u.end(), std::ostream_iterator<double>(std::cout, " "));
                    std::cout << "]";
                }
                std::cout << (iter_wo_improv == 0 ? " *\n" : "\n");
            }

            for (size_t i = 0; i < u.size(); i++) u[i] += mi*double(sg[i]);
        }
        return best_lb;
    }

    std::optional<TSPMH::TSPSolution> LagrangianTSP::extract_solution() const {
        return extract_solution(data);
    }

    std::optional<TSPMH::TSPSolution> LagrangianTSP::extract_solution(const Data& dp, int start) const {
        std::vector< std::vector<int> > adj_list(data.getDimension());
        for (const auto [v1, v2] : best_onetree) {
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
        assert(0 <= i && i < data.getDimension());
        assert(0 <= j && j < data.getDimension());
        for (int y = 0; y < data.getDimension(); y++) {
            std::swap(data.getMatrixCost()[i][y], data.getMatrixCost()[j][y]);
        }
        for (int x = 0; x < data.getDimension(); x++) {
            std::swap(data.getMatrixCost()[x][i], data.getMatrixCost()[x][j]);
        }
    }


    std::optional<TSPMH::TSPSolution> LagrangianTSP::optimize_and_extract_solution(Data& data, double ub, double epsilon_k,
            int max_iter, int max_iter_without_improv, int v0) {
        swap_data_vertices(data, v0, data.getDimension()-1);
        LagrangianTSP ltsp(data);
        ltsp.optimize(ub, epsilon_k, max_iter, max_iter_without_improv);
        swap_data_vertices(data, v0, data.getDimension()-1);
        return ltsp.extract_solution(data, data.getDimension()-1);
    }
}
