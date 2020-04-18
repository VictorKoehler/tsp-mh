#include "instancia.hpp"
#include <random>
#include <chrono>
#include <cmath>

namespace LegacyCVRP {
    
    std::default_random_engine generator;
    std::uniform_int_distribution<ullint> distribution;

    uint autoseed() {
        uint seed = std::chrono::system_clock::now().time_since_epoch().count();
        srand(seed);
        return seed;
    }

    void srand(uint seed) {
        generator = std::default_random_engine(seed);
    }

    void srand(ullint seed) {
        generator = std::default_random_engine(seed);
    }

    ullint mrand() {
        return distribution(generator);
    }

    ullint prand(std::default_random_engine &sp) {
        return distribution(sp);
    }

    void reset_vehicles_paths(Instancia *inst) {
        int zerocount = 0;
        for (uint i = 0; i < inst->path_len && zerocount < inst->vehicles; i++) {
            if (inst->path[i] == 0) {
                inst->vehicles_paths[zerocount++] = i;
            }
        }
    }



    // https://stackoverflow.com/questions/216823/whats-the-best-way-to-trim-stdstring
    const char* ws = " \t\n\r\f\v";

    // trim from end of string (right)
    inline std::string& rtrim(std::string& s, const char* t = ws)
    {
        s.erase(s.find_last_not_of(t) + 1);
        return s;
    }

    // trim from beginning of string (left)
    inline std::string& ltrim(std::string& s, const char* t = ws)
    {
        s.erase(0, s.find_first_not_of(t));
        return s;
    }

    // trim from both ends of string (right then left)
    inline std::string& trim(std::string& s, const char* t = ws)
    {
        return ltrim(rtrim(s, t), t);
    }

    Instancia *lerInstancia(FILE *arquivo, char *nome)
    {
        int d, v, c;
        fscanf(arquivo, "NAME: %s\n", nome);
        fscanf(arquivo, "DIMENSION: %d\n", &d);
        fscanf(arquivo, "VEHICLES: %d\n", &v);
        fscanf(arquivo, "CAPACITY: %d\n", &c);
        Instancia *inst = new Instancia(d, v, c);

        //int sumdem = 0;

        fscanf(arquivo, "DEMAND_SECTION:\n");
        for (int i = 0, ind; i < d; i++) //{
            fscanf(arquivo, "%d %d", &ind, &inst->demand[i]);
            //sumdem += inst->demand[i];
        //}

        char ops[30];
        fscanf(arquivo, "\n%s\n", ops);
        std::string opsstr(ops);
        
        //printf("%d*%d = %d %d\n", c, v, v*c, sumdem);

        if (trim(opsstr) == "EDGE_WEIGHT_SECTION") {
            //fscanf(arquivo, "\nEDGE_WEIGHT_SECTION\n");
            for (int i = 0; i < d; i++)
                for (int j = 0; j < d; j++)
                    fscanf(arquivo, "%d", &inst->edges_weight[i][j]);
        } else {
            int coordx[d], coordy[d], p;
            //fscanf(arquivo, "\nNODE_COORD_SECTION:n");
            for (int i = 0; i < d; i++)
            {
                fscanf(arquivo, "%d %d %d", &p, &coordx[i], &coordy[i]);
                // if (p != i) {
                //     printf("UNEXPECTED INPUT!");
                //     exit(2);
                // }
                for (int j = 0; j < i; j++) {
                    int dx = coordx[i] - coordx[j];
                    int dy = coordy[i] - coordy[j];
                    int dist = (int)round(sqrt(dx*dx+dy*dy));
                    inst->edges_weight[i][j] = dist;
                    inst->edges_weight[j][i] = dist;
                }
                inst->edges_weight[i][i] = 0;
            }
        }

        return inst;
    }



    void imprimir_caminho_simples(Instancia *inst, uint *path, bool print_nl) {
        if (print_nl)
            printf("\n");
        for (uint i = 0; i < inst->path_len; i++) {
            printf("%2d ", path[i]);
        }
        printf("\n");
    }

    void imprimir_caminho(Instancia *inst, uint *path) {
        printf("\n");
        for (uint i = 0; i < inst->path_len; i++) {
            printf("%2d ", path[i]);
        }
        printf("\n");
        char espacos[3*inst->path_len+1];
        for (uint i = 0; i < 3*inst->path_len; i++) {
            espacos[i] = ' ';
        }
        espacos[3*inst->path_len] = '\0';
        for (int i = 0; i < inst->vehicles; i++) {
            espacos[3*inst->vehicles_paths[i]+1] = '#';
        }
        printf("%s\n[", espacos);
        for (int i = 0; i < inst->vehicles; i++) {
            printf("%2d ", inst->vehicles_paths[i]);
        }
        printf("]\n");
    }

    void imprimir_resumo(Instancia *inst) {
        //printf("NAME: %s\n", inst->);
        printf("DIMENSION: %d\n", inst->dimension);
        printf("VEHICLES: %d\n", inst->vehicles);
        printf("CAPACITY: %d\n", inst->capacity);

        printf("DEMANDS:\n");
        for (int i = 0; i < inst->dimension; i++)
        {
            printf("%2d ", i);
        }
        printf("\n");
        for (int i = 0; i < inst->dimension; i++)
        {
            printf("%2d ", inst->demand[i]);
        }

        printf("\n\n    ");
        for (int i = 0; i < inst->dimension; i++)
        {
            printf("%2d ", i);
        }
        printf("\n");
        for (int i = 0; i < inst->dimension; i++)
        {
            printf("%2d: ", i);
            for (int j = 0; j < inst->dimension; j++)
                printf("%2d ", inst->edges_weight[i][j]);
            printf("\n");
        }
    }
}