#ifndef SIMULATOR_H
#define SIMULATOR_H

#include <random>
#include <boost/random.hpp>

using namespace std;

class Simulator {
private:
    mt19937 gen;
    random_device rd;
    default_random_engine generator;
    int ub;
    uniform_int_distribution<int> dist;

    static function<double(default_random_engine &)> get_distribution(const string &engine, double sigma);

    static string write_traces(double *tr_array, double *pt_array, uint32_t n_trc, unsigned int secret_key);

    pair<double *, double *> generate_traces_1d(
            uint32_t n_trc,
            unsigned int secret_key,
            const string &distribution_type,
            double sigma,
            unsigned int (*crypto_fun)(const unsigned int, const unsigned int),
            unsigned int (*lkg_fun)(const unsigned int)
    );

public:
    explicit Simulator();

    uint8_t generate_random_byte();

    string simulate_traces_1d(
            uint32_t n_trc,
            unsigned int secret_key,
            const string &distribution_type,
            double sigma,
            unsigned int (*crypto_fun)(const unsigned int, const unsigned int),
            unsigned int (*lkg_fun)(const unsigned int)
    );
};

#endif
