#ifndef __LEGACYCVRP_INST__
#define __LEGACYCVRP_INST__

#include <cstdio>
#include <string>

namespace LegacyCVRP {
    
    const int deposit = 0;

    typedef unsigned long long int ullint;
    typedef unsigned int uint;


    class Instancia
    {
    public:
        int dimension;
        int vehicles;
        int capacity;
        uint path_len, path_cost;
        int *demand;
        uint *path, *vehicles_paths;
        int **edges_weight;

        Instancia(int d, int v, int c)
        {
            printf("%d %d %d\n", d, v, c);
            dimension = d;
            vehicles = v;
            capacity = c;
            demand = new int[dimension];
            path_len = dimension + vehicles;
            path = new uint[path_len];
            vehicles_paths = new uint [vehicles];
            edges_weight = new int *[dimension];
            for (int i = 0; i < dimension; ++i)
                edges_weight[i] = new int[dimension];
        }

        ~Instancia()
        {
            for (int i = 0; i < dimension; ++i)
                delete[] edges_weight[i];
            delete[] edges_weight;
            delete[] vehicles_paths;
            delete[] demand;
            delete[] path;
        }
    };

    void reset_vehicles_paths(Instancia *inst);

    Instancia *lerInstancia(FILE *arquivo, char *nome);
    Instancia *lerInstancia(const std::string& arquivo);



    void imprimir_caminho_simples(Instancia *inst, uint *path, bool print_nl=true);

    void imprimir_caminho(Instancia *inst, uint *path);

    void imprimir_resumo(Instancia *inst);
}

#endif