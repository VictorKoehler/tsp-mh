#include "cvrpsolution.h"

namespace CVRPMH {
    SubRoutesIterable CVRPSolution::getSubRoutes() {
        return SubRoutesIterable(this);
    }
}