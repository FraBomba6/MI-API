#include "../include/simulator.h"
#include "../include/utils.h"

using namespace std;

/**
 * Simulator class constructor
 * Initializes the state variables
 */
Simulator::Simulator() {
    this->gen = mt19937(this->rd());
    this->generator = default_random_engine(this->rd());
    this->ub = (1UL << 20) - 1;
    this->dist = uniform_int_distribution<int>(0, ub);
}

/**
 * Generates a random byte using the uniform distribution
 * @return a random byte
 */
uint8_t Simulator::generate_random_byte() {
    return this->dist(this->gen) & 0xFF;
}

/**
 * Returns a distribution function based on the distribution type
 * @param distribution_type the distribution type
 * @param sigma the sigma parameter
 * @return a distribution function
 * @throws invalid_argument if the distribution type is invalid
 */
function<double(default_random_engine &)> Simulator::get_distribution(const string &distribution_type, double sigma) {
    if (distribution_type == "gauss") {
        std::normal_distribution<double> distribution_function(0, sigma);
        return [distribution_function](default_random_engine &engine) mutable { return distribution_function(engine); };
    } else if (distribution_type == "laplace") {
        boost::random::laplace_distribution<double> distribution_function(0, sigma);
        return [distribution_function](default_random_engine &engine) mutable { return distribution_function(engine); };
    } else {
        throw std::invalid_argument("Invalid distribution type");
    }
}

/**
 * Writes the traces to a file
 * @param tr_array the traces
 * @param n_trc the number of traces
 */
string Simulator::write_traces(double *tr_array, double *pt_array, uint32_t n_trc, unsigned int secret_key) {
    auto filename = MIUtils::generate_filename(n_trc);
    MIUtils::write_traces(filename, tr_array, pt_array, n_trc, secret_key);
    return filename;
}

/**
 * Generate a set of traces
 * @param n_trc the number of traces
 * @param secret_key the secret key
 * @param distribution_type the distribution type
 * @param sigma the sigma parameter
 * @param crypto_fun the cryptographic function
 * @param lkg_fun the leakage function
 * @return the traces
 */
pair<double *, double *> Simulator::generate_traces_1d(
        const uint32_t n_trc,
        const unsigned int secret_key,
        const string &distribution_type, const double sigma,
        unsigned int (*crypto_fun)(const unsigned int, const unsigned int),
        unsigned int (*lkg_fun)(const unsigned int)
) {
    auto distribution_function = this->get_distribution(distribution_type, sigma);
    uint8_t pt, ct, z;
    double tr;
    auto *pt_array = new double[n_trc];
    auto *tr_array = new double[n_trc];
    for (int i = 0; i < n_trc; i++) {
        pt = this->generate_random_byte();
        ct = crypto_fun(pt, secret_key);
        z = lkg_fun(ct);
        tr = z + distribution_function(this->generator);
        pt_array[i] = pt;
        tr_array[i] = tr;
    }
    return make_pair(pt_array, tr_array);
}

/**
 * Simulates a set of traces and writes them to a file
 * @param n_trc the number of traces
 * @param secret_key the secret key
 * @param distribution_type the distribution type
 * @param sigma the sigma parameter
 * @param crypto_fun the cryptographic function
 * @param lkg_fun the leakage function
 */
string Simulator::simulate_traces_1d(
        uint32_t n_trc,
        unsigned int secret_key,
        const string &distribution_type,
        double sigma,
        unsigned int (*crypto_fun)(const unsigned int, const unsigned int),
        unsigned int (*lkg_fun)(const unsigned int)
) {
    pair<double*, double*> pts_traces = this->generate_traces_1d(n_trc, secret_key, distribution_type, sigma, crypto_fun, lkg_fun);
    return this->write_traces(pts_traces.second, pts_traces.first, n_trc, secret_key);
}