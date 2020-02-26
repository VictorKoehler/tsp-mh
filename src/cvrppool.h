#ifndef __CVRPPOOL__
#define __CVRPPOOL__



#include <unordered_set>
#include <vector>

namespace CVRPMH
{
    // https://stackoverflow.com/questions/29855908/c-unordered-set-of-vectors
    struct VectorHash {
        size_t operator()(const std::pair<std::vector<int>, double>& v) const {
            std::hash<int> hasher;
            size_t seed = 0;
            for (int i : v.first) {
                seed ^= hasher(i) + 0x9e3779b9 + (seed<<6) + (seed>>2);
            }
            return seed;
        }
    };

    class CVRPPool {
        #ifdef DISABLE_CPLEX
        int inserted;
        #endif

        public:
        int lim, nveic, nclient;
        std::unordered_set<std::pair<std::vector<int>, double>, VectorHash> pool;

        CVRPPool(int nveiculos, int nclientes, int limit=-1) : lim(limit), nveic(nveiculos), nclient(nclientes), pool() {
            pool.reserve(limit+5);
            #ifdef DISABLE_CPLEX
            inserted = 0;
            #endif
        }

        void insert(std::vector<int>::iterator& srb, std::vector<int>::iterator& sre, double cost);

        void insert(std::vector<int> subroute, double cost);

        std::pair<std::vector<int>, double> commit();
    };
    
}

#endif