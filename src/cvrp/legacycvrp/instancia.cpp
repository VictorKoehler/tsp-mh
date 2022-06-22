#include "instancia.hpp"
#include <random>
#include <chrono>
#include <cmath>
#include <string>
#include <iostream>
#include <fstream>
#include <sstream>

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

    inline Instancia *new_instancia(const std::string& iname, int d, int v, int c) {
        if (v == -1 && iname.rfind("-k") != iname.npos) {
            v = std::stoi(iname.substr(iname.rfind("-k")+2));
        }
        if (d == -1 || c == -1 || v == -1) {
            std::cerr << "Instance file did not specify enough data: " << d << " " << v << " " << c << "\n";
            throw std::runtime_error("Instance file did not specify enough data.");
        }
        return new Instancia(d, v, c);
    }

    Instancia *lerInstancia(const std::string& arquivo) {
        std::ifstream infile;
        infile.open(arquivo);
        if (!infile.is_open()) {
            throw std::runtime_error("Could not open file '" + arquivo + "'.");
        }

        std::string iname;
        int d=-1, v=-1, c=-1;
        Instancia *inst = nullptr;

        while (infile.good()) {
            std::string buff;
            std::getline(infile, buff);
            trim(buff);
            if (buff.empty()) continue;

            auto hpos = buff.find(':');
            std::string h = buff.substr(0, hpos), tail = buff.substr(hpos == buff.npos ? buff.size() : hpos+1);
            trim(h);
            trim(tail);

            if (h == "NAME") iname = tail;
            else if (h == "DIMENSION") d = std::stoi(tail);
            else if (h == "VEHICLES") v = std::stoi(tail);
            else if (h == "CAPACITY") c = std::stoi(tail);
            else if (h == "DEMAND_SECTION") {
                if (inst == nullptr) inst = new_instancia(iname, d, v, c);
                for (int i = 0, p=0, ind; i < d; i++) {
                    infile >> ind >> inst->demand[i];
                    if (i == 0 && ind == 1) p = 1;
                    if (ind-p != i) throw std::runtime_error("Demand section must be from 1 to D: "+std::to_string(i+p)+"th parsing is "+std::to_string(ind));
                }
            } else if (h == "EDGE_WEIGHT_SECTION") {
                if (inst == nullptr) inst = new_instancia(iname, d, v, c);
                for (int i = 0; i < d; i++)
                    for (int j = 0; j < d; j++)
                        infile >> inst->edges_weight[i][j];
            } else if (h == "NODE_COORD_SECTION") {
                if (inst == nullptr) inst = new_instancia(iname, d, v, c);
                int coordx[d], coordy[d], p;
                for (int i = 0; i < d; i++)
                {
                    infile >> p >> coordx[i] >> coordy[i];
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
            } else if (h == "DEPOT_SECTION") {
                int d;
                while (infile >> d) {
                    if (d == -1) break;
                    else if (d != 1) throw std::runtime_error("Setting a custom depot id is not supported: " + std::to_string(d));
                }
            } else if (h != "COMMENT" && h != "TYPE" && h != "EDGE_WEIGHT_TYPE" && h != "EOF") {
                throw std::runtime_error("Unrecognized option: " + buff);
            }
        }

        return inst;
    }

    Instancia *lerInstancia(FILE *arquivo, char *nome)
    {
        int d, v, c, _t;
        _t = fscanf(arquivo, "NAME: %s\n", nome);
        _t = fscanf(arquivo, "DIMENSION: %d\n", &d);
        _t = fscanf(arquivo, "VEHICLES: %d\n", &v);
        _t = fscanf(arquivo, "CAPACITY: %d\n", &c);
        Instancia *inst = new Instancia(d, v, c);

        //int sumdem = 0;

        _t = fscanf(arquivo, "DEMAND_SECTION:\n");
        for (int i = 0, ind; i < d; i++) //{
            _t = fscanf(arquivo, "%d %d", &ind, &inst->demand[i]);
            //sumdem += inst->demand[i];
        //}

        char ops[30];
        _t = fscanf(arquivo, "\n%s\n", ops);
        std::string opsstr(ops);
        
        //printf("%d*%d = %d %d\n", c, v, v*c, sumdem);

        if (trim(opsstr) == "EDGE_WEIGHT_SECTION") {
            //_t = fscanf(arquivo, "\nEDGE_WEIGHT_SECTION\n");
            for (int i = 0; i < d; i++)
                for (int j = 0; j < d; j++)
                    _t = fscanf(arquivo, "%d", &inst->edges_weight[i][j]);
        } else {
            int coordx[d], coordy[d], p;
            //_t = fscanf(arquivo, "\nNODE_COORD_SECTION:n");
            for (int i = 0; i < d; i++)
            {
                _t = fscanf(arquivo, "%d %d %d", &p, &coordx[i], &coordy[i]);
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
        (void)_t; // do nothing with this bloody thing

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