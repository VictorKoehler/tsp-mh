#include <random>

std::minstd_rand simple_rand;

int pick_random(int excl_max) {
    return int(simple_rand()) % excl_max;
}

int pick_random(int incl_min, int excl_max) {
    return pick_random(excl_max - incl_min) + incl_min;
}

void set_random_seed(uint_fast32_t s) {
    simple_rand.seed(s);
}
