#include <vector>
#include "../cvrp/solution.h"
#include "../tsp/solution.h"

namespace HGSADC {

    class GiantTour {
        public:
        std::vector<int> tour;
        CVRPMH::CVRPContextProblemData *data;

        GiantTour() {}
        
        GiantTour(CVRPMH::CVRPContextProblemData *data) : data(data) {}
        
        GiantTour(CVRPMH::CVRPSolution& source);

        CVRPMH::CVRPSolution split_back();

        CVRPMH::CVRPSolution split_back_bellman_restricted_viability();

        CVRPMH::CVRPSolution split_back_bellman_penalized_viability(double wpen, int qmult = 2);
    };

}