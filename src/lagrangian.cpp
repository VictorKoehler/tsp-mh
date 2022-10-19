#include <limits>
#include <iterator>
#include <stack>
#include "global.h"
#include "lagrangian.h"
#include "prim.h"
#include "matrix.h"

namespace TSPLagrangian {
    LagrangianTSP::LagrangianTSP(const Data& data) : data(data), best_u(data.getDimension(), 0), best_lb(0) { }

    LagrangianTSP::LagrangianTSP(const LagrangianTSP& parent)
     : data(parent.data), disabled_arcs(parent.disabled_arcs), best_u(parent.best_u), best_lb(parent.best_lb) { }

    void LagrangianTSP::disable_arc(int i, int j) {
        disabled_arcs.emplace(i, j);
    }

    void LagrangianTSP::disable_edge(int i, int j) {
        disable_arc(i, j);
        disable_arc(j, i);
    }

    bool LagrangianTSP::is_arc_disabled(int i, int j) const {
        return disabled_arcs.find(std::make_pair(i, j)) != disabled_arcs.end();
    }

    template<bool zero_disabled>
    struct Auxiliary2DCostMatrix {
        const LagrangianTSP& tsp;
        const std::vector<double>& u;

        Auxiliary2DCostMatrix(const LagrangianTSP& tsp, const std::vector<double>& u) : tsp(tsp), u(u) {}
        inline int size() const { return tsp.getData().getDimension(); }
        inline double operator()(int i, int j) const {
            if constexpr (zero_disabled) {
                if (i == 0 || j == 0) return std::numeric_limits<double>::max();
            }
            assert(tsp.getData()(i, j) == tsp.getData()(j, i));
            return tsp.is_arc_disabled(i, j) ? std::numeric_limits<double>::max() : tsp.getData()(i, j) - u[i] - u[j];
        }
    };
    
    double LagrangianTSP::optimize(double ub, double epsilon_k, int max_iter, int max_iter_without_improv) {
        // Simple2DMatrix<double> cost_matrix(data.getDimension()-1);
        std::vector<double> u;
        GraphEdges onetree;

        best_lb = -std::numeric_limits<double>::infinity();
        u.swap(best_u);

        if (Verbosity >= 2) {
            double su = 0;
            for (const auto ui : u) su += ui;
            std::cout << "LAGRANGIAN OPTIMIZATION: " << max_iter << " maximum iterations, " << max_iter_without_improv << " without improvements, ub="
                        << ub << ", e_k=" << epsilon_k << ", sum(u)=" << su << "\n";
        }

        if (max_iter == -1) max_iter = std::numeric_limits<int>::max();
        if (max_iter_without_improv == -1) max_iter_without_improv = std::numeric_limits<int>::max();

        for (int iter = 1, iter_wo_improv = 0; iter <= max_iter && iter_wo_improv <= max_iter_without_improv; iter++) {
            // for (int i = 0; i < cost_matrix.size(); i++) // last is left out
            //     for (int j = 0; j < cost_matrix.size(); j++) // last is left out
            //         cost_matrix(i, j) = is_arc_disabled(i, j) ? std::numeric_limits<double>::max() : data(i, j) - u[i] - u[j];
            
            const Auxiliary2DCostMatrix<true> cost_matrix(*this, u);
            onetree = prims_algorithm(cost_matrix, 1);
            assert(std::get<1>(onetree.back()) == 0);
            onetree.pop_back();
            assert(std::get<1>(onetree.back()) != 0);

            const Auxiliary2DCostMatrix<false> cost(*this, u);
            int first_edge = 1, second_edge = 2, locked_vert = 0;
            if (cost(locked_vert, first_edge) > cost(locked_vert, second_edge))
                std::swap(first_edge, second_edge);
            
            for (int w = 3; w < cost.size(); w++) {// connects last
                if (cost(locked_vert, w) < cost(locked_vert, second_edge)) {
                    second_edge = w;
                    if (cost(locked_vert, w) < cost(locked_vert, first_edge))
                        std::swap(first_edge, second_edge);
                }
            }
            onetree.emplace_back(locked_vert, first_edge);
            onetree.emplace_back(locked_vert, second_edge);

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


            int sg_norm = 0;
            for (const auto sg_i : sg) sg_norm += sg_i*sg_i;

            double mi = epsilon_k*(ub-nz)/double(sg_norm);

            if (Verbosity >= 4) {
                std::cout << "LAGR: " << iter << ", z'=" << nz << ", mi=" << mi
                          << ", e_k=" << epsilon_k << ", |sg'|=" << sg_norm;
                if (Verbosity >= 5) {
                    std::cout << ", sg'=[";
                    std::copy(sg.begin(), sg.end(), std::ostream_iterator<int>(std::cout, " "));
                    std::cout << "], u'=[";
                    std::copy(u.begin(), u.end(), std::ostream_iterator<double>(std::cout, " "));
                    std::cout << "]";
                }
                std::cout << (nz > best_lb ? " *\n" : "\n");
            }

            if (nz > best_lb) {
                best_lb = nz;
                best_u = u;
                best_onetree.swap(onetree);
                if (sg_norm == 0) return nz;
                iter_wo_improv = 0;
            } else if (sg_norm == 0) {
                throw std::runtime_error("OLA!");
            } else {
                iter_wo_improv++;
            }

            for (size_t i = 0; i < u.size(); i++) u[i] += mi*double(sg[i]);
        }
        return best_lb;
    }

    std::optional<TSPMH::TSPSolution> LagrangianTSP::extract_solution() const {
        std::vector< std::vector<int> > adj_list(data.getDimension());
        return extract_solution(adj_list);
    }

    std::optional<TSPMH::TSPSolution> LagrangianTSP::extract_solution(std::vector< std::vector<int> >& adj_list) const {
        return extract_solution(getData(), adj_list, 0);
    }

    std::optional<TSPMH::TSPSolution> LagrangianTSP::extract_solution(const Data& dp, std::vector< std::vector<int> >& adj_list, int start) const {
        if (best_onetree.empty()) throw std::runtime_error("Not optimized");
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

}
