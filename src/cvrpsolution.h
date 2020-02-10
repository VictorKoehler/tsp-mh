#ifndef __CVRPSOLUTION_CLASS__
#define __CVRPSOLUTION_CLASS__

#include "tspsolution.h"
#include "legacycvrp/construtores.hpp"

namespace CVRPMH {

    class SubRoute;

    class CVRPSolution : public TSPMH::TSPSolution {
        template<class T>
        static inline T** qMatrTInit(int d) {
            T** r = new T *[d];
            for (int i = 0; i < d; ++i)
                r[i] = new T[d];
            return r;
        }

        public:

        int vehicles, capac;
        std::vector<int> demand, subroutes, subcapacity;
        bool isSubRoute;


        CVRPSolution() {}

        CVRPSolution(uint d, double** m) : TSPSolution(d, m) { }

        CVRPSolution(uint d, double** m, int* dem, int c, int v) : TSPSolution(d, m), vehicles(v), capac(c) {
            isSubRoute = false;
            demand.assign(dem, dem+d);
            subroutes.resize(v);
            subcapacity.resize(v);
        }

        CVRPSolution(LegacyCVRP::Instancia* inst)
            : CVRPSolution(inst->dimension, qMatrTInit<double>(inst->dimension), inst->demand, inst->capacity, inst->vehicles) {
            assign(inst->path, inst->path + inst->path_len);
            for (uint i = 0; i < dimension; i++) {
                for (uint j = 0; j < dimension; j++) {
                    matrizAdj[i][j] = double(inst->edges_weight[i][j]);
                }
            }
            updateSubRoutes();
            update_cost();
        }

        inline void cpy(const CVRPSolution& obj) {
            vehicles = obj.vehicles;
            capac = obj.capac;
            demand = obj.demand;
            subroutes = obj.subroutes;
            subcapacity = obj.subcapacity;
            isSubRoute = obj.isSubRoute;
            dimension = obj.dimension;
            cost = obj.cost;
            matrizAdj = obj.matrizAdj;
        }

        CVRPSolution(const CVRPSolution& obj) : TSPSolution(obj) {
            cpy(obj);
        }

        CVRPSolution& operator=(const CVRPSolution& obj) {
            TSPSolution::operator=(obj);
            cpy(obj);
            return *this;
        }

        void updateSubRoutes() {
            for (size_t i = 0; i < subcapacity.size(); i++) {
                subcapacity[i] = 0;
            }
            subroutes[0] = 0;
            for (int i = 1, a = 0; size_t(i) < size()-1; i++) {
                if (at(i) == 0) subroutes[++a] = i;
                else subcapacity[a] += demand[at(i)];
            }
        }

        bool checkSolution() {
            bool visited[dimension]={false};
            uint ccvisited = 0;
            int c=0, ind=0;
            for (auto a : *this) {
                ind++;
                if (a == CVRPSolution::route_start) {
                    if (c > capac) {
                        printf("checkSolution: Overcapacity (%d/%d) on %dth pos was visited.\n", c, capac, ind);
                        return false;
                    }
                    c = 0;
                }
                c += demand[a];
                if (visited[a] && a != CVRPSolution::route_start) {
                    printf("checkSolution: %d repeated.", a);
                    return false;
                } else if (!visited[a]) {
                    visited[a] = true;
                    ccvisited++;
                }
            }
            return ccvisited == dimension;
        }
    };



    class SubRoute : public CVRPSolution {
        int _b, _e, _i;
        TSPSolution* src;

        inline TSPMH::vecit b() const noexcept { return src->it(_b); };
        inline TSPMH::vecit e() const noexcept { return src->it(_e); };
    public:
        // ...
        SubRoute(TSPSolution* src, int b, int e)
            : CVRPSolution(src->dimension, src->matrizAdj), _b(b), _e(e), src(src) {
            cost = 0;
            isSubRoute = true;
        }

        SubRoute(CVRPSolution* src, int i)
            : SubRoute(src, src->subroutes[i], size_t(i+1) < src->subroutes.size() ? 1+src->subroutes[i+1] : src->size()) {
            cpy(*src);
            cost = 0;
            isSubRoute = true;
            capac = src->subcapacity[i];
            vehicles = 1;
            _i = i;
        }

        inline void setCapacity(int c) {
            static_cast<CVRPSolution*>(src)->subcapacity[_i] = c;
        }

        TSPMH::vecit begin() noexcept override { return b(); }
        TSPMH::vecit end() noexcept override   { return e(); }
        typename std::iterator_traits<TSPMH::vecit>::reference
        operator[](std::size_t index) { return b()[index]; }
        int &at(std::size_t index) { return b()[index]; }
        void push_back(int x) { src->insert(e(), x); }
        TSPMH::vecit insert(const TSPMH::vecit p, int i)  { return src->insert(p, i); }
        TSPMH::vecit insert(const TSPMH::vecit p, const TSPMH::vecit b, const TSPMH::vecit e)  { return src->insert(p, b, e); }
        TSPMH::vecit erase(const TSPMH::vecit b, const TSPMH::vecit e)  { return src->erase(b, e); }
        std::size_t size() const noexcept { return std::distance(b(), e()); }
        TSPMH::vecit it(std::size_t ind) override { return b() + ind; }
        TSPMH::vecit it(int ind) override { return b() + ind; }
    };


}

#endif