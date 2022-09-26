#include <vector>
#include <optional>
#include "tspsolution.h"
#include "matrix.h"

namespace TSPBaB {
    class LagrangianTSP {
        private:
        const Data& data;
        Simple2DMatrix<double> cost_matrix;
        std::vector<double> u;
        std::vector<std::tuple<int, int>> onetree;

        inline double cost(int i, int j) {
            if (i == cost_matrix.size()) return data(i, j) - u[i] - u[j];
            return cost_matrix(i, j);
        }

        std::optional<TSPMH::TSPSolution> extract_solution(const Data& dp, int start = 0);

        static void swap_data_vertices(Data& data, int i, int j);

        public:
        LagrangianTSP(const Data& data);

        double optimize(double ub, double epsilon_k, int max_iter=-1);

        std::optional<TSPMH::TSPSolution> extract_solution();

        static std::optional<TSPMH::TSPSolution> optimize_and_extract_solution(Data& data, double ub, double epsilon_k, int max_iter=-1, int v0 = 0);
    };
}