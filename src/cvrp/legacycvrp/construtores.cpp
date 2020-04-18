#include "construtores.hpp"
#include <cstdio>
#include <limits>
#include <queue>
#include <stack>
#include <set>

using namespace std;

namespace LegacyCVRP {


    const int MINFINITY = __INT_MAX__;

    typedef pair<int, int> parint;
    typedef priority_queue<parint, vector< parint >, greater< parint > > pqueueMin;
    typedef priority_queue<parint, vector< parint >, less< parint > > pqueueMax;


    int greedy_constructor(Instancia *inst) {
        int max_path = inst->path_len;
        int visited_count = 1, vehicle_ind = 1, totalCost = 0;
        bool visited[inst->dimension] = {false};
        int vehicles_paths[inst->vehicles + 1] = {0};
        
        for (int i = 0; i < inst->path_len; i++) {
            inst->path[i] = deposit;
        }

        /* PRIORITY QUEUE */
        parint menor_adj_elementos[inst->dimension];
        for (int j = 0; j < inst->dimension; j++) {
            menor_adj_elementos[j] = make_pair(inst->edges_weight[deposit][j], j);
        }
        menor_adj_elementos[deposit] = make_pair(MINFINITY, deposit);
        pqueueMin menor_adj(menor_adj_elementos, menor_adj_elementos + inst->dimension); // (cost of deposit->index, index)

        pqueueMax buckets; // (number of elements on route, routeindex)
        pqueueMin buckets_by_demmand; // (demmand of route, routeindex)
        /* PRIORITY QUEUE */

        // while (!menor_adj[0].empty()) {
        //     printf("%d ", menor_adj[0].top().first);
        //     menor_adj[0].pop();
        // }
        // printf("\n");

        visited[deposit] = true;
        for (int i = 0; i < inst->vehicles; i++) {
            int lastVisited = deposit, lastDemmand = 0, lastCost = 0, sumdemand = 0, costpath = 0, startedat = vehicle_ind;

            pair<int, int> first;
            do {
                //printf("%s(%2d), ", visited[next.second] ? (inst->demand[next.second] + sumdemand > inst->capacity ? "both" : "visd") : "over", next.second);
                first = menor_adj.top();
                menor_adj.pop();
            } while (visited[first.second] && !menor_adj.empty());
            if (visited[first.second]) {
                break;
            }
            lastVisited = first.second;
            lastCost = first.first;
            lastDemmand = inst->demand[lastVisited];

            while (true) {
                //printf("Choosen %2d (cost: %2d;   demand: %2d)\n", lastVisited, lastCost, inst->demand[lastVisited]);
                inst->path[vehicle_ind] = lastVisited;
                visited[lastVisited] = true;
                sumdemand += lastDemmand;
                costpath += lastCost;
                vehicle_ind++;


                if (lastVisited != deposit) {
                    visited_count++;

                    int bestind = 0, bestcost = MINFINITY, bestdem = 0;
                    for (int k = 1; k < inst->dimension; k++) {
                        if (!visited[k] && inst->edges_weight[lastVisited][k] < bestcost &&
                                        inst->demand[k] + sumdemand <= inst->capacity) {
                            bestind = k;
                            bestcost = inst->edges_weight[lastVisited][k];
                            bestdem = inst->demand[k];
                        }
                    }
                    if (bestind == 0) {
                        bestcost = inst->edges_weight[lastVisited][deposit];
                    }
                    lastVisited = bestind;
                    lastCost = bestcost;
                    lastDemmand = bestdem;
                }
                else {
                    break;
                }
            }

            //printf("Cost of route #%d: %2d; Capacity: %2d\n\n", i, costpath, sumdemand);
            vehicles_paths[i] = startedat;
            buckets.push(make_pair(vehicle_ind - startedat, i));
            buckets_by_demmand.push(make_pair(sumdemand, i));
            totalCost += costpath;
        }
        inst->path_cost = totalCost;




        /** 
         * Quando o construtor guloso construir uma solução inválida (isto é, que sobre clientes não atendidos),
         * esse trecho será acionado para distribuir os clientes em alguma solução válida, porém ineficiente.
         */
        if (visited_count != inst->dimension) {
            // OH SHIT!
            vehicles_paths[inst->vehicles] = vehicle_ind;
            pqueueMax items; // (Demmand of index, index)
            bool route_used[inst->vehicles] = {false}; // index
            bool bucketOfDemmands = false;

            uint vis = inst->dimension - visited_count, visi = 0, itercc = 0;
            while (items.size() != vis) {
                if (!visited[visi]) {
                    items.push(make_pair(inst->demand[visi], visi));
                }
                visi++;
            }

            while (!buckets.empty()) {
                int route, numelem;
                if (bucketOfDemmands) {
                    route = buckets_by_demmand.top().second;
                    numelem = vehicles_paths[route + 1] - vehicles_paths[route];
                    buckets_by_demmand.pop();
                }
                else {
                    route = buckets.top().second;
                    numelem = buckets.top().first;
                    buckets.pop();
                }
                if (route_used[route]) {
                    continue;
                }

                bucketOfDemmands = !bucketOfDemmands;
                route_used[route] = true;
                //printf("Cant do it! Adding %d to the baliza! %lu elems\n", route, items.size());

                for (int t, i = vehicles_paths[route]; i < vehicles_paths[route] + numelem; i++) {
                    t = inst->path[i];
                    if (t != deposit)
                        items.push(make_pair(inst->demand[t], t));
                }

                if (itercc > 0) {
                    pqueueMax cpitems(items); // (Demmand of index, index)
                    int maxbuckets = itercc+1;
                    int buckets_cost[maxbuckets] = {0};
                    vector<int> new_buckets[maxbuckets];
                    bool sucess = true;

                    while (!cpitems.empty()) {
                        int demmand = cpitems.top().first;
                        int vert = cpitems.top().second;
                        int currentBucket = 0;
                        cpitems.pop();

                        while (buckets_cost[currentBucket] + demmand > inst->capacity && currentBucket < maxbuckets) {
                            currentBucket++;
                        }
                        if (currentBucket == maxbuckets) {
                            sucess = false;
                            break;
                        } else {
                            new_buckets[currentBucket].push_back(vert);
                            buckets_cost[currentBucket] += demmand;
                            //printf("Added %d (%d) to #%d bucket (now %d).\n", vert, demmand, currentBucket, buckets_cost[currentBucket]);
                        }
                    }




                    if (sucess) {
                        int insert_ind = 1;
                        inst->path_cost = 0;
                        for (int r = 0; r < inst->vehicles; r++) {
                            if (!route_used[r] && vehicles_paths[r] != insert_ind) {
                                //printf("copying route %d:", r);
                                for (int r_in = vehicles_paths[r]; r_in < vehicles_paths[r + 1]; r_in++, insert_ind++) {
                                    inst->path[insert_ind] = inst->path[r_in];
                                    inst->path_cost += inst->edges_weight[inst->path[insert_ind - 1]][inst->path[r_in]];
                                    //printf(" %d", inst->path[insert_ind]);
                                }
                                //printf("\n");
                            }
                        }
                        
                        for (int m = 0; m < maxbuckets; m++) {
                            //printf("adding route:");
                            for (auto el : new_buckets[m]) {
                                inst->path_cost += inst->edges_weight[inst->path[insert_ind - 1]][el];
                                inst->path[insert_ind++] = el;
                                //printf(" %d", el);
                            }
                            inst->path_cost += inst->edges_weight[inst->path[insert_ind - 1]][deposit];
                            inst->path[insert_ind++] = deposit;
                            //printf("\n");
                        }
                        break;
                    }
                }
                itercc++;
            }

            reset_vehicles_paths(inst);
        }
        else {
            reset_vehicles_paths(inst);
            // for (int i = 0; i < inst->vehicles; i++) {
            //     inst->vehicles_paths[i] = vehicles_paths[i] == 0 ? 0 : vehicles_paths[i]-1;
            // }
        }

        return inst->dimension - visited_count;
    }
}