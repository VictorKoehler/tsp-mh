#include <set>
#include "global.h"

using namespace std;

#include "cvrppool.h"
#ifndef DISABLE_CPLEX
#include "cplex_utils.h"
#endif

namespace CVRPMH
{

    inline bool shouldInsert(size_t size, double cost, CVRPPool* p) {
        if (p->lim != -1 && p->pool.size() >= size_t(p->lim)) {
            int i = pick_random(int(p->pool.size()));
            auto t = p->pool.begin();
            while (i--) t++;
            auto dd = t->first;
            auto ds = t->second.size();
            if (dd/double(ds) > cost/double(size)) {
                p->pool.erase(t);
                dprintf("Pool Overflow! Replaced random subroute{%ld clients, %lf cost} by {%ld clients, %lf cost}\n", ds, dd, size, cost);
                return true;
            }
            dprintf("Pool Overflow! Subroute{%ld clients, %lf cost} discarted\n", size, cost);
            return false;
        }
        return true;
    }

    void CVRPPool::insert(vector<int>::iterator& srb, vector<int>::iterator& sre, double cost) {
        if (shouldInsert(std::distance(srb, sre), cost, this)) {
            #ifdef DISABLE_CPLEX
            if (pool.find(make_pair(vector<int>(srb, sre), cost)) == pool.end())
                inserted++;
            #endif
            pool.insert(make_pair(cost, vector<int>(srb, sre)));
        }
    }

    void CVRPPool::insert(vector<int> subroute, double cost) {
        if (shouldInsert(subroute.size(), cost, this)) {
            #ifdef DISABLE_CPLEX
            if (pool.find(make_pair(subroute, cost)) == pool.end())
                inserted++;
            #endif
            pool.insert(make_pair(cost, subroute));
        }
    }

    // Pool Permutation Enrichment Algorithm
    #ifdef PPEA

    template<typename T>
    class ordered_fixed_size_set {
        size_t msz;
        public:
        set<T> cont;

        ordered_fixed_size_set(size_t max_size) : msz(max_size) { }

        size_t max_size() const { return msz; }

        void set_max_size(size_t new_max_size) const {
            msz = new_max_size;
            if (new_max_size < cont.size()) {
                auto tend = cont.end();
                auto trem = tend-(cont.size()-new_max_size);
                cont.erase(trem, tend);
            }
        }

        void insert(T &&__x) {
            cont.insert(__x);
            if (cont.size() > msz)
                cont.erase(cont.end()-1);
        }

        void insert(const T &__x) {
            cont.insert(__x);
            if (cont.size() > msz)
                cont.erase(cont.end()-1);
        }
    };

    void pool_permutation_enrichment_algorithm(CVRPPool* pool) {
        ordered_fixed_size_set< pair< double, vector<int> > > subpool(4*pool->pool.size());

    }
    #endif
    
    pair<vector<int>, double> CVRPPool::commit() {
        #ifdef PPEA

        #endif

        #ifndef DISABLE_CPLEX
        size_t sz = pool.size();
        // bool presente[sz][nclient] = {false};
        std::vector< std::vector<bool> > presente(nclient, std::vector<bool>(sz, false));
        double custos[sz];

        dprintf("Pool costs:");
        int i = 0;
        for (const auto& s : pool) {
            for (auto c : s.second) {
                presente[c][i] = true;
            }
            ifdebug(VectorHash v);
            dprintf(" %.0lf(%lu)", s.first, v(s));
            custos[i] = s.first;
            i++;
        }
        dprintf("\n");

        // Ambiente
        IloEnv env;

        // Criando um modelo
        IloModel modelo(env);
        // CPLEX_MUTE(env);



        // Variáveis de decisão

        ////////////////////////////////////////////////////////
        // Variável Xr
        // Se a rota r está sendo usada
        ////////////////////////////////////////////////////////
        IloBoolVarArray x(env, sz);

        // Adicao da variável x ao modelo
        for (size_t j = 0; j < sz; j++) {
            char var[100];
            sprintf(var, "x(%ld)", j);
            x[j].setName(var);
            modelo.add(x[j]);
        }


        // Função Objetivo
        IloExpr SumDX(env);
        for (size_t j = 0; j < sz; j++)
            SumDX += custos[j]*x[j];

        // Adicionando a FO
        modelo.add(IloMinimize(env, SumDX));



        // Restrição: Cada cliente aparece uma única vez na solução
        // Com exceção do depósito.
        for (int c = 1; c < nclient; ++c) {
            IloExpr temp(env); // Se o cliente está na subrota...
            for (size_t r = 0; r < sz; r++) {
                temp += presente[c][r]*x[r];
            }
            modelo.add(temp == 1); // 5.24
            temp.end();
        }

        // Existem exatamente K (num. de veículos) rotas ativas.
        modelo.add(IloSum(x) <= nveic);



        // Let the games begin
        IloCplex cvrpModel(modelo);

        IloNum startTime;
        startTime = cvrpModel.getTime();
        cvrpModel.solve();

        cout << "STATUS: " <<  cvrpModel.getCplexStatus() << endl;
        cout << "BEST: " <<  cvrpModel.getBestObjValue() << endl;
        cout << "OBJ VALUE: " <<  cvrpModel.getObjValue() << endl;
        cout << "TIME ELAPSED: " << (cvrpModel.getTime()-startTime) << endl << endl << "-----" << endl;

        double finalcost = cvrpModel.getBestObjValue();
        vector<int> finalroute;

        IloNumArray xarr(env, sz);
        cvrpModel.getValues(x, xarr);
        i = 0;
        for (const auto& s : pool) {
            if (xarr[i++] > 0.98)
                finalroute.insert(finalroute.end(), s.second.begin(), s.second.end()-1);
        }
        finalroute.resize(nclient + nveic, 0);
        
        env.end();
        return make_pair(finalroute, finalcost);

        #else
        printf("Solutions present: %ld / %d\n", pool.size(), inserted);
        #endif
    }
}
