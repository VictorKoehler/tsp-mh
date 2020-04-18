#ifndef __CVRPHEUR_CLASS__
#define __CVRPHEUR_CLASS__

#include "solution.h"

namespace CVRPMH {

    class CVRPW_BI_RVND_DB {
        public:
        typedef CVRPSolution Solution;
        
        LegacyCVRP::Instancia *inst;

        CVRPW_BI_RVND_DB(LegacyCVRP::Instancia *inst) : inst(inst) { }

        Solution construct();

        void localsearch(Solution* sol);

        Solution pertubation(Solution* sol);
    };

    void rvnd(CVRPSolution* candidate);

    CVRPSolution gils_rvnd(std::unique_ptr<LegacyCVRP::Instancia> inst);
    
}

#endif