#include <vector>
#include <optional>
#include <unordered_set>
#include "tspsolution.h"

namespace TSPLagrangian {
    class LagrangianTSP {
        typedef std::vector<std::tuple<int, int>> GraphEdges;
        private:
        const Data& data;
        
        struct pair_hash {
            template <class T1, class T2>
            inline std::size_t operator()(const std::pair<T1, T2> &p) const {
                std::size_t seed = 0;
                seed ^= std::hash<T1>()(p.first) + 0x9e3779b9 + (seed << 6) + (seed >> 2);
                seed ^= std::hash<T2>()(p.second) + 0x4B2CCCB5 + (seed << 6) + (seed >> 2);
                return seed;
            }
        };

        std::unordered_set<std::pair<int, int>, pair_hash> disabled_arcs;
        std::vector<double> best_u;
        GraphEdges best_onetree;
        double best_lb;

        std::optional<TSPMH::TSPSolution> extract_solution(const Data& dp, std::vector< std::vector<int> >& adj_list, int start) const;

        static void swap_data_vertices(Data& data, int i, int j);

        public:
        LagrangianTSP(const Data& data);
        LagrangianTSP(const LagrangianTSP& parent);

        void disable_arc(int i, int j);
        void disable_edge(int i, int j);
        bool is_arc_disabled(int i, int j) const;
        std::size_t getAmountOfDisabledArcs() const { return disabled_arcs.size(); }

        double optimize(double ub, double epsilon_k, int max_iter=-1, int max_iter_without_improv=-1);

        std::optional<TSPMH::TSPSolution> extract_solution() const;
        std::optional<TSPMH::TSPSolution> extract_solution(std::vector< std::vector<int> >& adj_list) const;

        const std::vector<double>& getSolutionLambda() const { return best_u; }
        const GraphEdges& getSolutionOneTree() const { return best_onetree; }
        double getSolutionLB() const { return best_lb; }
        const Data& getData() const { return data; }
    };
}