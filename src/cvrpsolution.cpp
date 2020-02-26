#include "cvrpsolution.h"

#define assertOrReturn(statement, autoassert) { if (!autoassert) return statement; else { assert(statement); }}
#define autoassertOrReturn(statement) assertOrReturn(statement, autoassert)

namespace CVRPMH {

    SubRoutesIterable CVRPSolution::getSubRoutes() {
        return SubRoutesIterable(this);
    }

    bool CVRPSolution::checkSolution(bool autoassert) {
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
        autoassertOrReturn(ccvisited == dimension);
    }
}