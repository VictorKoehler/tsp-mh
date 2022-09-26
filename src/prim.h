#ifndef __PRIMS_ALGORITHM__
#define __PRIMS_ALGORITHM__

#include <vector>
#include <queue>
#include <tuple>
#include <type_traits>

namespace TSPBaB {
    template<typename MatrixLike>
    std::vector< std::tuple<int, int> > prims_algorithm(const MatrixLike& mat) {
        using P = std::tuple<double, int, int>;

        std::vector< std::tuple<int, int> > ret;
        const int s = int(mat.size());
        int visited_cc = 0;
        std::vector<bool> visited(s, false);
        std::priority_queue<P, std::vector<P>, std::greater<P>> q;

        q.push(std::make_tuple(0, -1, 0));
        while (visited_cc < s && !q.empty()) {
            auto t = q.top();
            q.pop();
            const auto& vi = std::get<2>(t);
            if (visited[vi]) continue;
            if (std::get<1>(t) != -1) ret.emplace_back(std::get<1>(t), vi);
            visited[vi] = true;
            visited_cc++;

            for (int vj = 0; vj < s; vj++) {
                if (visited[vj]) continue;
                double m;
                if constexpr (std::is_invocable_r_v<double, decltype(mat), int, int>) {
                    m = mat(vi, vj);
                } else {
                    m = mat[vi][vj];
                }
                if (m >= 0) q.push(std::make_tuple(m, vi, vj));
            }
        }
        return ret;
    }
}

#endif