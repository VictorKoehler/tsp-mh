#include "solution.h"

#define assertOrReturn(statement, autoassert) { if (!autoassert) return statement; else { assert(statement); }}
#define autoassertOrReturn(statement) assertOrReturn(statement, autoassert)

#include <numeric>

namespace CVRPMH {

    RoutesIterable CVRPSolution::getRoutes() {
        return RoutesIterable(this);
    }

    CVRPRoute CVRPSolution::getRoute(int index) {
        return CVRPRoute(this, index);
    }

    bool CVRPSolution::checkSolution(bool autoassert, bool complete) {
        bool visited[data->dimension]={false};
        uint ccvisited = 0;
        int c=0, ind=0;
        size_t routeind=0;
        for (auto a : *this) {
            if (a == CVRPSolution::route_start) {
                if (routeind < subroutes.size() && subroutes[routeind++] != ind)
                    autoassertOrReturn(false);
                if (c > data->maxcapacity) {
                    printf("checkSolution: Overcapacity (%d/%d) on %dth pos was visited.\n", c, data->maxcapacity, ind);
                    autoassertOrReturn(false);
                }
                c = 0;
            }
            c += data->demand[a];
            if (visited[a] && a != CVRPSolution::route_start) {
                printf("checkSolution: %d repeated.", a);
                autoassertOrReturn(false);
            } else if (!visited[a]) {
                visited[a] = true;
                ccvisited++;
            }
            ind++;
        }
        autoassertOrReturn(!complete || ccvisited == data->dimension);
        return true;
    }

    inline int getsubrindex_shifted(CVRPSolution *s, int p) {
        if (s->at(p) == CVRPSolution::route_start)
            return s->getRouteIndex(p-1);
        else return s->getRouteIndex(p);
    }

    double CVRPSolution::insertion_cost(uint n, int p) {
        if (subcapacity[getsubrindex_shifted(this, p)] + data->demand[n] > data->maxcapacity)
            return INFINITYLF;
        return TSPSolution::insertion_cost(n, *(it(p)-1), *it(p));
    }

    void CVRPSolution::insert_candidate(int c, int pos) {
        cost += insertion_cost(c, pos);
        auto subind = size_t(getsubrindex_shifted(this, pos));
        subcapacity[subind] += data->demand[c];
        auto subrsz = subroutes.size();
        for (auto i = subind+1; i < subrsz; i++) {
            subroutes[i] += 1;
        }
        insert(it(pos), c);
    }

    void CVRPSolution::updateRoutes() {
        for (size_t i = 0; i < subcapacity.size(); i++) {
            subcapacity[i] = 0;
        }
        subroutes[0] = 0;
        for (size_t i = 1, a = 0; i < size()-1; i++) {
            if (at(i) == 0) {
                a++;
                assert(a != subroutes.size());
                subroutes[a] = i;
            }
            else subcapacity[a] += data->demand[at(i)];
        }
    }

    void CVRPSolution::copy(const CVRPSolution& obj, int deep) {
        isSubRoute = obj.isSubRoute;
        cost = obj.cost;
        data = obj.data;
        TSPSolution::data = obj.data->getTSPPtr();
        assert(static_cast<TSPSolution*>(this)->data->dimension ==
               data->dimension);
        if (deep) {
            subroutes = obj.subroutes;
            subcapacity = obj.subcapacity;
        }
    }
}