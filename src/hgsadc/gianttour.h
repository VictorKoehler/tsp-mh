#include <vector>
#include "../cvrp/solution.h"
#include "../tsp/solution.h"

class GiantTour {
    public:
    std::vector<int> tour;

    GiantTour() {}
    
    GiantTour(CVRPMH::CVRPSolution& source);

    CVRPMH::CVRPSolution split_back(CVRPMH::CVRPSolution& data_base);
};
