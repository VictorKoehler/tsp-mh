#include <numeric>
#include <set>
#include <tuple>
#include <cmath>
#include <algorithm>

#include "heur.h"
#include "constructive.h"
#include "cplex_utils.h"

using namespace std;


#define INITIAL_SUBTOUR_SIZE 1
#define INITIAL_SUBTOUR_ALFA 0.5

#define it(i) begin() + (i)

namespace CVRPMH {

    CVRPSolution GreedyDummyConstructor::construct() {
        LegacyCVRP::greedy_constructor(inst);
        return CVRPSolution(inst, data);
    }

    void bestinsertion_backup_model_sumup(TSPMH::TSPSolution& sol, vector<int>& candidatos) {
        sol.update_cost();

        while (!candidatos.empty()) {
            set< tuple<double, size_t, size_t> > custoInsercao;
            size_t curr_sz = sol.size()-1;
            int maxtamp = floor(double(curr_sz*candidatos.size())*INITIAL_SUBTOUR_ALFA);
            size_t choose = size_t(TSPMH::_random(maxtamp));

            for (size_t pos = 1; pos < sol.size(); pos++) {
                for (size_t c = 0; c < candidatos.size(); c++) {
                    custoInsercao.insert(make_tuple(sol.insertion_cost(candidatos[c], sol.it(pos)), c, pos));
                    if (custoInsercao.size() > choose + 1) {
                        custoInsercao.erase(--custoInsercao.end());
                    }
                }
            }

            auto cand = *--custoInsercao.end();
            sol.insert_candidate(candidatos[get<1>(cand)], get<2>(cand));
            candidatos.erase(candidatos.it(get<1>(cand)));
        }
    }
    
    void reajusta_rotas(CVRPSolution& sol, vector<int>& faltantes) {
        
        int rota_original_clientes[sol.data->dimension];
        int rotaind = -1;
        for (auto cliente : sol) {
            if (cliente != CVRPSolution::route_start) {
                rota_original_clientes[cliente] = rotaind;
            } else {
                rotaind++;
            }
        }
        
        for (auto f : faltantes) {
            rota_original_clientes[f] = -1;
        }


        // Ambiente
        IloEnv env;

        // Criando um modelo
        IloModel modelo(env);
        CPLEX_MUTE(env);

        // Variáveis de decisão

        ////////////////////////////////////////////////////////////////////////////
        // Variável Mi
        // True se o cliente i está na mesma rota que foi designado originalmente
        ////////////////////////////////////////////////////////////////////////////
        IloBoolVarArray m(env, sol.data->dimension);

        // Adicao da variável M ao modelo
        for (uint j = 1; j < sol.data->dimension; j++) {
            char var[100];
            sprintf(var, "M(%d)", j);
            m[j].setName(var);
            modelo.add(m[j]);
        }

        ////////////////////////////////////////////////////////////////////////////
        // Variável Cir
        // True se o cliente i está na rota r.
        ////////////////////////////////////////////////////////////////////////////
        IloArray<IloBoolVarArray> c(env, sol.data->dimension);
        for (uint i = 1; i < sol.data->dimension; ++i) {
            c[i] = IloBoolVarArray(env, sol.data->vehicles);
        }

        // Adicao da variável Cir ao modelo
        for (uint i = 1; i < sol.data->dimension; i++) {
            for (int r = 0; r < sol.data->vehicles; r++) {
                char var[100];
                sprintf(var, "c(%d,%d)", i, r);
                c[i][r].setName(var);
                modelo.add(c[i][r]);
            }
        }

        // Adicionando a FO
        modelo.add(IloMaximize(env, IloSum(m)));



        // Restrição: Cada cliente aparece uma única vez na solução
        // Com exceção do depósito.
        for (uint i = 1; i < sol.data->dimension; i++) {
            modelo.add(IloSum(c[i]) == 1); // (I)
        }

        // Restrição: Cada rota tem sua capacidade.
        for (int r = 0; r < sol.data->vehicles; r++) {
            IloExpr temp(env);
            for (uint i = 1; i < sol.data->dimension; i++) {
                temp += sol.data->demand[i]*c[i][r];
            }
            modelo.add(temp <= sol.data->maxcapacity); // (II)
            temp.end();
        }

        // Restrição: Cir é 1 quando o cliente i está na rota r.
        // Quando Mi for 0, Cir pode ser 0 ou 1.
        // Quando Mi for 1, Cir só pode ser 1.
        // Então, quando dizemos que r = rota original de i, fazendo Mi = 1 (problema de maximização),
        // forçamos que a rota escolhida seja a rota original. Quando Mi = 0, a rota pode ser qualquer coisa.
        for (uint i = 1; i < sol.data->dimension; i++) {
            if (rota_original_clientes[i] == -1) continue;

            modelo.add(c[i][rota_original_clientes[i]] >= m[i]); // (III)
        }


        // Let the games begin
        IloCplex cvrpModel(modelo);
        cvrpModel.setParam(IloCplex::Param::MIP::Limits::Solutions, 1);

        IloNum startTime;
        startTime = cvrpModel.getTime();
        cvrpModel.solve();

        cout << "STATUS: " <<  cvrpModel.getCplexStatus() << endl;
        cout << "BEST: " <<  cvrpModel.getBestObjValue() << endl;
        cout << "OBJ VALUE: " <<  cvrpModel.getObjValue() << endl;
        cout << "TIME ELAPSED: " << (cvrpModel.getTime()-startTime) << endl << endl << "-----" << endl;



        vector<IloNumArray> val_c(sol.data->dimension);
        for (uint i = 1; i < sol.data->dimension; ++i) {
            val_c[i] = IloNumArray(env, sol.data->vehicles);
            cvrpModel.getValues(c[i], val_c[i]);
        }

        // Brand new copy, because we're iterating over original
        vector<int> newsol(1, 0);

        for (int r = 0; r < sol.data->vehicles; r++) {
            vector<int> newpushes;
            uint routesz = 0;
            for (uint i = 1; i < sol.data->dimension; i++) {
                if (val_c[i][r] > 0.98) {
                    routesz++;
                    if (rota_original_clientes[i] != r)
                        newpushes.push_back(i);
                }
            }

            TSPMH::TSPSolution newroute(sol.data->getTSPPtr());
            for (auto o : sol.getRoute(r)) {
                if (o != 0 && val_c[o][r] > 0.98) {
                    newroute.insert(newroute.end()-1, o);
                }
            }

            bestinsertion_backup_model_sumup(newroute, newpushes);
            newsol.insert(newsol.end(), newroute.it(1), newroute.end());
        }

        sol.swap(newsol);
        sol.update_cost();
        sol.updateRoutes();

        env.end();
    }

    CVRPSolution BestInsertionConstructor::construct() {
        CVRPSolution sol(inst, data, false);
        sol.resize(sol.data->vehicles+1, 0);
        vector<int> candidatos(sol.data->dimension - 1);
        iota(candidatos.begin(), candidatos.end(), 1);
        random_shuffle(candidatos.begin(), candidatos.end());

        for (int i = 1; i <= INITIAL_SUBTOUR_SIZE; i++) {
            int r;
            do {
                r = TSPMH::_random(candidatos.size());
            } while (sol.insertion_cost(candidatos[r], i) == INFINITYLF);
            sol.insert_candidate(candidatos[r], i);
            candidatos.erase(candidatos.it(r));
        }
        sol.update_cost();
        sol.updateRoutes();
        assert(sol.checkSolution(true, false));

        while (!candidatos.empty()) {
            set< tuple<double, size_t, size_t> > custoInsercao;
            size_t curr_sz = sol.size()-1;
            int maxtamp = floor(double(curr_sz*candidatos.size())*INITIAL_SUBTOUR_ALFA);
            size_t choose = size_t(TSPMH::_random(maxtamp));

            for (size_t pos = 1; pos < sol.size(); pos++) {
                for (size_t c = 0; c < candidatos.size(); c++) {
                    const auto t = make_tuple(sol.insertion_cost(candidatos[c], pos), c, pos);
                    if (get<0>(t) != INFINITYLF) {
                        custoInsercao.insert(t);
                        if (custoInsercao.size() > choose + 1) {
                            custoInsercao.erase(--custoInsercao.end());
                        }
                    }
                }
            }

            if (custoInsercao.empty()) break;
            auto cand = *--custoInsercao.end();
            sol.insert_candidate(candidatos[get<1>(cand)], get<2>(cand));
            assert(sol.checkSolution(true, false));
            candidatos.erase(candidatos.it(get<1>(cand)));
        }

        if (!candidatos.empty()) {
            reajusta_rotas(sol, candidatos);
        }
        assert(sol.checkSolution(true));
        return sol;
    }
}