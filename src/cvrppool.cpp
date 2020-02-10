#include "cvrppool.h"
#include "cplex_utils.h"

using namespace std;

#ifndef dprintf
#define dprintf(...)
// #define dprintf(...) printf(__VA_ARGS__)
#endif

namespace CVRPMH
{

    inline int _random(int excl_max) {
        return rand() % excl_max;
    }

    inline bool shouldInsert(size_t size, double cost, CVRPPool* p) {
        if (p->lim != -1 && p->pool.size() >= size_t(p->lim)) {
            int i = _random(int(p->pool.size()));
            auto t = p->pool.begin();
            while (i--) t++;
            auto dd = t->second;
            auto ds = t->first.size();
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
        if (shouldInsert(std::distance(srb, sre), cost, this))
            pool.insert(make_pair(vector<int>(srb, sre), cost));
    }

    void CVRPPool::insert(vector<int> subroute, double cost) {
        if (shouldInsert(subroute.size(), cost, this))
            pool.insert(make_pair(subroute, cost));
    }
    
    pair<vector<int>, double> CVRPPool::commit() {
        size_t sz = pool.size();
        bool presente[sz][nclient];
        double custos[sz];

        int i = 0;
        for (auto s : pool) {
            for (auto c : s.first) {
                presente[i][c] = true;
            }
            custos[i] = s.second;
            i++;
        }

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
                if (presente[r][c]) temp += x[r];
            }
            modelo.add(temp == 1); // 5.24
            temp.end();
        }

        // Existem exatamente K (num. de veículos) rotas ativas.
        modelo.add(IloSum(x) == nveic);



        // Let the games begin
        IloCplex cvrpModel(modelo);

        IloNum startTime;
        startTime = cvrpModel.getTime();
        cvrpModel.solve();

        cout << "BEST: " <<  cvrpModel.getBestObjValue() << endl;
        cout << "OBJ VALUE: " <<  cvrpModel.getObjValue() << endl;
        cout << "STATUS: " <<  cvrpModel.getCplexStatus() << endl;
        cout << "TIME ELAPSED: " << (cvrpModel.getTime()-startTime) << endl << endl << "-----" << endl;

        double finalcost = cvrpModel.getBestObjValue();
        vector<int> finalroute;
        i = 0;
        for (auto s : pool) {
            if (cvrpModel.getValue(x[i]) > 0.98)
                finalroute.insert(finalroute.end(), s.first.begin(), s.first.end()-1);
        }
        finalroute.push_back(0);
        
        env.end();
        return make_pair(finalroute, finalcost);
    }
}