#ifndef __CVRPCPDATA_CLASS__
#define __CVRPCPDATA_CLASS__

#include <vector>

#include "../tsp/contextdata.h"
#include "legacycvrp/instancia.hpp"

namespace CVRPMH {

    class CVRPContextProblemData : public TSPMH::TSPContextProblemData {
        bool cleanup;
        const static int default_mc = 0xFFFFFFF;        
    public:
        int vehicles, maxcapacity;
        std::vector<int> demand;

        CVRPContextProblemData(uint d, double** m, int v, int mc, int* dem)
            : TSPContextProblemData(d, m), vehicles(v), maxcapacity(mc), cleanup(false) {
            if (dem != NULL)
                demand.assign(dem, dem + d);
        }

        CVRPContextProblemData(TSPContextProblemData* import, int v = 1, int mc = default_mc)
            : CVRPContextProblemData(import->dimension, import->matrizAdj, v, mc, NULL) {
        }

        CVRPContextProblemData(LegacyCVRP::Instancia* inst)
            : CVRPContextProblemData(inst->dimension, NULL, inst->vehicles, inst->capacity, inst->demand) {
                matrizAdj = new double*[dimension];
                for (int i = 0; i < dimension; ++i)
                    matrizAdj[i] = new double[dimension];
                cleanup = true;

                for (uint i = 0; i < dimension; i++) {
                    for (uint j = 0; j < dimension; j++) {
                        matrizAdj[i][j] = double(inst->edges_weight[i][j]);
                    }
                }
        }
        
        ~CVRPContextProblemData() {
            if (cleanup) {
                for (int i = 0; i < dimension; ++i)
                    delete [] matrizAdj[i];
                delete [] matrizAdj;
            }
        }

        TSPMH::TSPContextProblemData* getTSPPtr() {
            return this;
        }
    };

}  // namespace CVRPMH
#endif