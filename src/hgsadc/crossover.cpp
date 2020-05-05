#include "crossover.h"
#include "global.h"

using namespace std;

namespace HGSADC {

    vector<int> pix_vector(CVRPMH::CVRPSolution& first, CVRPMH::CVRPSolution& second) {
        vector<vector<int>> p(first.data->vehicles);
        bool visited[first.data->dimension] = {false};

        int rindex = 0;
        for (const auto& r : first.getRoutes()) {
            p[rindex].reserve(r.size());
            auto  a = pick_random(0, r.size()+1), b = pick_random(0, r.size()+1);
            if (a > b) swap(a, b);
            for (auto i = a; i < b; i++) {
                if (r[i] == CVRPMH::CVRPSolution::route_start) continue;
                p[rindex].push_back(r[i]);
                visited[r[i]] = true;
            }
            rindex++;
        }

        vector<int> g;
        g.reserve(second.size());
        g.push_back(CVRPMH::CVRPSolution::route_start);
        rindex = 0;
        for (auto r : second.getRoutes()) {
            for (auto c : r) {
                if (c == CVRPMH::CVRPSolution::route_start || visited[c]) continue;
                p[rindex].push_back(c);
            }
            g.insert(g.end(), p[rindex].begin(), p[rindex].end());
            rindex++;
        }

        return g;
    }


    GiantTour pix(CVRPMH::CVRPSolution& a, CVRPMH::CVRPSolution& b) {
        auto gt = GiantTour(a.data);
        gt.tour = std::move(pix_vector(a, b));
        return gt;
    }
}