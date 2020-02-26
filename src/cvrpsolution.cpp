#include "cvrpsolution.h"

#define assertOrReturn(statement, autoassert) { if (!autoassert) return statement; else { assert(statement); }}
#define autoassertOrReturn(statement) assertOrReturn(statement, autoassert)

#include <numeric>
const double INFINITYLF = std::numeric_limits<double>::infinity();

namespace CVRPMH {

    SubRoutesIterable CVRPSolution::getSubRoutes() {
        return SubRoutesIterable(this);
    }

    bool CVRPSolution::checkSolution(bool autoassert, bool complete) {
        bool visited[dimension]={false};
        uint ccvisited = 0;
        int c=0, ind=0;
        size_t routeind=0;
        for (auto a : *this) {
            if (a == CVRPSolution::route_start) {
                if (routeind < subroutes.size() && subroutes[routeind++] != ind)
                    autoassertOrReturn(false);
                if (c > maxcapacity) {
                    printf("checkSolution: Overcapacity (%d/%d) on %dth pos was visited.\n", c, maxcapacity, ind);
                    autoassertOrReturn(false);
                }
                c = 0;
            }
            c += demand[a];
            if (visited[a] && a != CVRPSolution::route_start) {
                printf("checkSolution: %d repeated.", a);
                autoassertOrReturn(false);
            } else if (!visited[a]) {
                visited[a] = true;
                ccvisited++;
            }
            ind++;
        }
        autoassertOrReturn(!complete || ccvisited == dimension);
        return true;
    }

    inline int getsubrindex_shifted(CVRPSolution *s, int p) {
        if (s->at(p) == CVRPSolution::route_start)
            return s->getSubRouteIndex(p-1);
        else return s->getSubRouteIndex(p);
    }

    double CVRPSolution::insertion_cost(uint n, int p) {
        if (subcapacity[getsubrindex_shifted(this, p)] + demand[n] > maxcapacity)
            return INFINITYLF;
        return TSPSolution::insertion_cost(n, *(it(p)-1), *it(p));
    }

    void CVRPSolution::insert_candidate(int c, int pos) {
        cost += insertion_cost(c, pos);
        auto subind = getsubrindex_shifted(this, pos);
        subcapacity[subind] += demand[c];
        auto subrsz = subroutes.size();
        for (int i = subind+1; i < subrsz; i++) {
            subroutes[i] += 1;
        }
        insert(it(pos), c);
    }
}