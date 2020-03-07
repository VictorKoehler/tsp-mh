#ifndef __CVRPSOLUTION_CLASS__
#define __CVRPSOLUTION_CLASS__

#include <algorithm>
#include "tspsolution.h"
#include "legacycvrp/construtores.hpp"

namespace CVRPMH {

    class RoutesIterable;

    class CVRPSolution : public TSPMH::TSPSolution {
        template<class T>
        static inline T** qMatrTInit(int d) {
            T** r = new T *[d];
            for (int i = 0; i < d; ++i)
                r[i] = new T[d];
            return r;
        }

        public:

        int vehicles, maxcapacity;
        std::vector<int> demand, subroutes, subcapacity;
        bool isSubRoute;


        CVRPSolution() {}

        CVRPSolution(uint d, double** m) : TSPSolution(d, m) { }

        CVRPSolution(uint d, double** m, int* dem, int c, int v) : TSPSolution(d, m), vehicles(v), maxcapacity(c) {
            isSubRoute = false;
            demand.assign(dem, dem+d);
            subroutes.resize(v);
            subcapacity.resize(v);
        }

        CVRPSolution(LegacyCVRP::Instancia* inst, bool importRoute = true)
            : CVRPSolution(inst->dimension, qMatrTInit<double>(inst->dimension), inst->demand, inst->capacity, inst->vehicles) {
            for (uint i = 0; i < dimension; i++) {
                for (uint j = 0; j < dimension; j++) {
                    matrizAdj[i][j] = double(inst->edges_weight[i][j]);
                }
            }
            if (importRoute) {
                assign(inst->path, inst->path + inst->path_len);
                updateRoutes();
                update_cost();
            }
        }

        inline void cpy(const CVRPSolution& obj) {
            vehicles = obj.vehicles;
            maxcapacity = obj.maxcapacity;
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

        void updateRoutes() {
            for (size_t i = 0; i < subcapacity.size(); i++) {
                subcapacity[i] = 0;
            }
            subroutes[0] = 0;
            for (int i = 1, a = 0; size_t(i) < size()-1; i++) {
                if (at(i) == 0) subroutes[++a] = i;
                else subcapacity[a] += demand[at(i)];
            }
        }

        inline int getRouteIndex(int solindex) {
            auto a = std::upper_bound(subroutes.begin(), subroutes.end(), solindex);
            return a - subroutes.begin() - 1;
        }

        inline int getRouteIndex(TSPMH::vecit& it) {
            return getRouteIndex(distance(begin(), it));
        }

        inline int getRouteIndex(const TSPMH::vecit& it) {
            return getRouteIndex(distance(begin(), it));
        }

        RoutesIterable getRoutes();

        bool checkSolution(bool autoassert=false, bool complete=true);

        double insertion_cost(uint n, int p);

        void insert_candidate(int c, int pos);
    };



    class CVRPRoute : public CVRPSolution {
        int _b, _e, _i;
        TSPSolution* src;

        inline TSPMH::vecit b() const noexcept { return src->it(_b); };
        inline TSPMH::vecit e() const noexcept { return src->it(_e); };

        CVRPRoute(CVRPSolution* src, int i)
            : CVRPRoute(src, src->subroutes[i], size_t(i+1) < src->subroutes.size() ? 1+src->subroutes[i+1] : src->size()) {
            cpy(*src);
            cost = 0;
            isSubRoute = true;
            maxcapacity = src->maxcapacity;
            vehicles = 1;
            _i = i;
        }
    public:
        // ...
        CVRPRoute(TSPSolution* src, int b, int e)
            : CVRPSolution(src->dimension, src->matrizAdj), _b(b), _e(e), src(src) {
            cost = 0;
            isSubRoute = true;
        }

        inline void setCapacity(int c) {
            static_cast<CVRPSolution*>(src)->subcapacity[_i] = c;
        }

        bool operator!=(const CVRPRoute & other) const { return _b != other._b; }

        TSPMH::vecit begin() noexcept override { return b(); }
        TSPMH::vecit end() noexcept override   { return e(); }
        typename std::iterator_traits<TSPMH::vecit>::reference
        operator[](std::size_t index) override { return b()[index]; }
        int &at(std::size_t index) override { return b()[index]; }
        void push_back(int x) override { src->insert(e(), x); }
        TSPMH::vecit insert(const TSPMH::vecit p, int i)  override { return src->insert(p, i); }
        TSPMH::vecit insert(const TSPMH::vecit p, const TSPMH::vecit b, const TSPMH::vecit e)  override { return src->insert(p, b, e); }
        TSPMH::vecit erase(const TSPMH::vecit b, const TSPMH::vecit e)  override { return src->erase(b, e); }
        std::size_t size() noexcept override { return std::distance(b(), e()); }
        TSPMH::vecit it(std::size_t ind) override { return b() + ind; }
        TSPMH::vecit it(int ind) override { return b() + ind; }

        static CVRPRoute importfrom(CVRPSolution* src, int i) {
            return CVRPRoute(src, i);
        }
    };

    class RoutesIterable {
        public:
        class iterator {
            private:
            CVRPSolution* src;
            unsigned ptr;

            public:
            iterator(CVRPSolution* src, unsigned ptr): src(src), ptr(ptr){}
            iterator operator++() { ++ptr; return *this; }
            bool operator!=(const iterator & other) const { return ptr != other.ptr; }
            const CVRPRoute operator*() const { return CVRPRoute::importfrom(src, ptr); }
        };

        private:
        CVRPSolution* src;

        public:
        RoutesIterable(CVRPSolution* src) : src(src) { }
        iterator begin() const { return iterator(src, 0); }
        iterator end() const { return iterator(src, unsigned(src->vehicles)); }
    };

}

#endif