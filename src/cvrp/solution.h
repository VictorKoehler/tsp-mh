#ifndef __CVRPSOLUTION_CLASS__
#define __CVRPSOLUTION_CLASS__

#include <algorithm>
#include "../tsp/solution.h"
#include "legacycvrp/construtores.hpp"
#include "contextdata.h"

namespace CVRPMH {

    class CVRPRoute;

    class RoutesIterable;

    class CVRPSolution : public TSPMH::TSPSolution {
        public:
        std::vector<int> subroutes, subcapacity;
        bool isSubRoute;
        CVRPContextProblemData *data;

        CVRPSolution() {}

        CVRPSolution(CVRPContextProblemData *data) : TSPSolution(data->getTSPPtr()), data(data) {
            isSubRoute = false;
            subroutes.resize(data->vehicles);
            subcapacity.resize(data->vehicles);
        }

        CVRPSolution(LegacyCVRP::Instancia* inst, CVRPContextProblemData *data, bool importRoute = true) : CVRPSolution(data) {
            if (importRoute) {
                assign(inst->path, inst->path + inst->path_len);
                updateRoutes();
                update_cost();
            }
        }

        CVRPSolution(const CVRPSolution& obj) : TSPSolution(obj) {
            copy(obj);
        }

        ~CVRPSolution() {
            if (destroydata) {
                delete data;
                destroydata = false;
            }
        }

        CVRPSolution& operator=(const CVRPSolution& obj) {
            TSPSolution::operator=(obj);
            copy(obj);
            return *this;
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

        void updateRoutes();

        RoutesIterable getRoutes();

        CVRPRoute getRoute(int index);

        bool checkSolution(bool autoassert=false, bool complete=true);

        double insertion_cost(uint n, int p);

        void insert_candidate(int c, int pos);

        void copy(const CVRPSolution& obj, int deep=true);
    };



    class CVRPRoute : public CVRPSolution {
        int _b, _e, _i;
        TSPSolution* src;
        CVRPContextProblemData holder;

        inline TSPMH::vecit b() const noexcept { return src->it(_b); };
        inline TSPMH::vecit e() const noexcept { return src->it(_e); };
    public:
        int capacity;

        CVRPRoute(TSPSolution* src, int b, int e)
            : CVRPSolution(), _b(b), _e(e), src(src), holder(src->data) {
            cost = 0;
            capacity = 0;
            isSubRoute = true;
            data = &holder;
            TSPSolution::data = holder.getTSPPtr();
        }

        CVRPRoute(CVRPSolution* src, int i)
            : CVRPRoute(src, src->subroutes[i], size_t(i+1) < src->subroutes.size() ? 1+src->subroutes[i+1] : src->size()) {
            copy(*src, false);
            _i = i;
            isSubRoute = true;
            capacity = src->subcapacity[_i];
            cost = 0;
        }

        inline void setCapacity(int c) {
            capacity = c;
            static_cast<CVRPSolution*>(src)->subcapacity[_i] = c;
        }

        bool operator!=(const CVRPRoute & other) const { return _b != other._b; }

        TSPMH::vecit begin() noexcept override { return b(); }
        TSPMH::vecit end() noexcept override   { return e(); }
        const int operator[](std::size_t index) const noexcept override { return b()[index]; }
        int& operator[](std::size_t index) noexcept override { return b()[index]; }
        int &at(std::size_t index) override { return b()[index]; }
        void push_back(int x) override { src->insert(e(), x); }
        TSPMH::vecit insert(const TSPMH::vecit p, int i)  override { return src->insert(p, i); }
        TSPMH::vecit insert(const TSPMH::vecit p, const TSPMH::vecit b, const TSPMH::vecit e)  override { return src->insert(p, b, e); }
        TSPMH::vecit erase(const TSPMH::vecit b, const TSPMH::vecit e)  override { return src->erase(b, e); }
        std::size_t size() const noexcept override { return std::distance(b(), e()); }
        TSPMH::vecit it(std::size_t ind) override { return b() + ind; }
        TSPMH::vecit it(int ind) override { return b() + ind; }
    };

    class RoutesIterable {
        public:
        class iterator {
            private:
            CVRPSolution* src;
            int ptr;

            public:
            iterator(CVRPSolution* src, int ptr): src(src), ptr(ptr){}
            iterator operator++() { ++ptr; return *this; }
            bool operator!=(const iterator & other) const { return ptr != other.ptr; }
            const CVRPRoute operator*() const { return CVRPRoute(src, ptr); }
        };

        private:
        CVRPSolution* src;

        public:
        RoutesIterable(CVRPSolution* src) : src(src) { }
        iterator begin() const { return iterator(src, 0); }
        iterator end() const { return iterator(src, unsigned(src->data->vehicles)); }
    };

}

#endif